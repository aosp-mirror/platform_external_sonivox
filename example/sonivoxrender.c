/*
 * Copyright (c) 2022, Pedro López-Cabanillas
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <eas.h>
#include <eas_reverb.h>

#define UNUSED(X) (void)X

EAS_I32 reverb_type = -1;
EAS_I32 reverb_wet = 0;
uint32_t mBase = 0;
int64_t mLength = 0;
int mFd = 0;

static int readFunction(void *handle, void *buffer, int offset, int size) {
    UNUSED(handle);
    if (offset > mLength) offset = mLength;
    lseek(mFd, mBase + offset, SEEK_SET);
    if (offset + size > mLength) {
        size = mLength - offset;
    }
    return read(mFd, buffer, size);
}

static int sizeFunction(void *handle) {
    UNUSED(handle);
    return mLength;
}

int renderFile(char *fileName)
{
    EAS_DATA_HANDLE mEASDataHandle = NULL;
    EAS_HANDLE mEASStreamHandle = NULL;
    EAS_FILE mEasFile;
    EAS_PCM *mAudioBuffer = NULL;
    EAS_I32 mPCMBufferSize = 0;
    const S_EAS_LIB_CONFIG *mEASConfig;

    struct stat statbuf;
    int ok = EXIT_SUCCESS;
    int err = 0;

    mFd = open(fileName, O_RDONLY);
    if (mFd < 0) {
        fprintf(stderr, "Failed to open %s. error: %s\n", fileName, strerror(errno));
        ok = EXIT_FAILURE;
        return ok;
    }

    err = stat(fileName, &statbuf);
    if (err < 0) {
        fprintf(stderr, "Failed to stat %s. error: %s\n", fileName, strerror(errno));
        ok = EXIT_FAILURE;
        return ok;
    }

    mBase = 0;
    mLength = statbuf.st_size;
    mEasFile.handle = fileName;
    mEasFile.readAt = readFunction;
    mEasFile.size = sizeFunction;

    EAS_RESULT result = EAS_Init(&mEASDataHandle);
    if (result != EAS_SUCCESS) {
        fprintf(stderr, "Failed to initialize synthesizer library\n");
        ok = EXIT_FAILURE;
        goto cleanup;
    }

    if (mEASDataHandle == NULL) {
        fprintf(stderr, "Failed to initialize EAS data handle\n");
        ok = EXIT_FAILURE;
        goto cleanup;
    }
    
    result = EAS_SetParameter(mEASDataHandle, EAS_MODULE_REVERB, EAS_PARAM_REVERB_WET, reverb_wet);
    if (result != EAS_SUCCESS) {
        fprintf(stderr, "Failed to set reverb wet amount");
        ok = EXIT_FAILURE;
        goto cleanup;
    }
    
    EAS_BOOL sw = EAS_TRUE;
    EAS_I32 preset = reverb_type - 1;
    if ( preset >= EAS_PARAM_REVERB_LARGE_HALL && preset <= EAS_PARAM_REVERB_ROOM ) {
        sw = EAS_FALSE;
        result = EAS_SetParameter(mEASDataHandle, EAS_MODULE_REVERB, EAS_PARAM_REVERB_PRESET, preset);
        if (result != EAS_SUCCESS) {
            fprintf(stderr, "Failed to set reverb preset");
            ok = EXIT_FAILURE;
            goto cleanup;
        }
    }
    result = EAS_SetParameter(mEASDataHandle, EAS_MODULE_REVERB, EAS_PARAM_REVERB_BYPASS, sw);
    if (result != EAS_SUCCESS) {
        fprintf(stderr, "Failed to set reverb bypass");
        ok = EXIT_FAILURE;
        goto cleanup;
    }    

    result = EAS_OpenFile(mEASDataHandle, &mEasFile, &mEASStreamHandle);
    if (result != EAS_SUCCESS) {
        fprintf(stderr, "Failed to open file\n");
        ok = EXIT_FAILURE;
        goto cleanup;
    }

    if(mEASStreamHandle == NULL) {
        fprintf(stderr, "Failed to initialize EAS stream handle\n");
        ok = EXIT_FAILURE;
        goto cleanup;
    }

    result = EAS_Prepare(mEASDataHandle, mEASStreamHandle);
    if (result != EAS_SUCCESS) {
        fprintf(stderr, "Failed to prepare EAS data and stream handles\n");
        ok = EXIT_FAILURE;
        goto cleanup;
    }

    mEASConfig = EAS_Config();
    if (mEASConfig == NULL) {
        fprintf(stderr, "Failed to get the library configuration\n");
        ok = EXIT_FAILURE;
        goto cleanup;
    }

    mPCMBufferSize = sizeof(EAS_PCM) * mEASConfig->mixBufferSize * mEASConfig->numChannels;
    mAudioBuffer = alloca(mPCMBufferSize);
    if (mAudioBuffer == NULL) {
        fprintf(stderr, "Failed to allocate memory of size: %ld", mPCMBufferSize);
        ok = EXIT_FAILURE;
        goto cleanup;
    }

    while (1) {
        EAS_STATE state;
        EAS_I32 count = -1;

        result = EAS_State(mEASDataHandle, mEASStreamHandle, &state);
        if (result != EAS_SUCCESS) {
            fprintf(stderr, "Failed to get EAS State\n");
            ok = EXIT_FAILURE;
            break;
        }

        if (state == EAS_STATE_STOPPED || state == EAS_STATE_ERROR) {
            break; /* playback is complete */
        }

        result = EAS_Render(mEASDataHandle, mAudioBuffer, mEASConfig->mixBufferSize, &count);
        if (result != EAS_SUCCESS) {
            fprintf(stderr, "Failed to render audio\n");
            ok = EXIT_FAILURE;
            break;
        }
        
        if (count != mEASConfig->mixBufferSize) {
            fprintf(stderr, "Only %ld out of %ld frames rendered\n", count, mEASConfig->mixBufferSize);
            ok = EXIT_FAILURE;
            break;
        }
        
        fwrite(mAudioBuffer, sizeof(EAS_PCM), mEASConfig->mixBufferSize * mEASConfig->numChannels, stdout);
        fflush(stdout);
    }
    
cleanup:
    if (mEASStreamHandle) {
        result = EAS_CloseFile(mEASDataHandle, mEASStreamHandle);
        if (result != EAS_SUCCESS) {
            fprintf(stderr, "Failed to close audio file/stream\n");
            ok = EXIT_FAILURE;
        }
    }
    
    if (mEASDataHandle) {
        result = EAS_Shutdown(mEASDataHandle);
        if (result != EAS_SUCCESS) {
            fprintf(stderr, "Failed to deallocate the resources for synthesizer library\n");
            ok = EXIT_FAILURE;
        }
    }
    
    if (mFd > 0) {
        close(mFd);
    }
    return ok;
}

int main (int argc, char **argv)
{
    int ok = EXIT_SUCCESS;
    int index, c;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, "hr:w:")) != -1) {
        switch (c)
        {
        case 'h':
            fprintf (stderr, "Usage: %s [-h] [-r 0..4] [-w 0..32765] file.mid ...\n"\
                        "Render standard MIDI files into raw PCM audio.\n"\
                        "Options:\n"\
                        "\t-h\tthis help message\n"\
                        "\t-r n\treverb preset: 0=no, 1=large hall, 2=hall, 3=chamber, 4=room\n"\
                        "\t-w n\treverb wet: 0..32765\n", argv[0]);
            return EXIT_FAILURE;
        case 'r':
            reverb_type = atoi(optarg);
            if (reverb_type < 0 || reverb_type > 4) {
                fprintf (stderr, "invalid reverb preset: %ld\n", reverb_type);
                return EXIT_FAILURE;
            }
            break;
        case 'w':
            reverb_wet = atoi(optarg);
            if (reverb_wet < 0 || reverb_wet > 32765) {
                fprintf (stderr, "invalid reverb amount: %ld\n", reverb_wet);
                return EXIT_FAILURE;
            }
            break;
        default:
            fprintf (stderr, "unknown option: %c\n", optopt);
            return EXIT_FAILURE;
        }
    }

    for (index = optind; index < argc; index++) {
        ok = renderFile(argv[index]);
        if (ok != EXIT_SUCCESS) {
            break;
        }
    }

    return ok;
}
