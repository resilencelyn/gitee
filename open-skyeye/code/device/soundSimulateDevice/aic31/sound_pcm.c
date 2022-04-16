/*
	skyeye_sound_pcm.c - pcm module of sound simulation for skyeye
	Copyright (C) 2007 Anthony Lee <don.anthony.lee@gmail.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
*/

/*
 * 03/25/2007	Written by Anthony Lee
 */

/*
 * WARNING: uncompleted yet !!!
 */

#include "skyeye_sound.h"

#if (defined(__MINGW32__) || defined(__CYGWIN__))

#undef WORD
#include <windows.h>

struct pcm_sound_t
{
	CRITICAL_SECTION locker;

	HWAVEIN handle_in;
	HWAVEOUT handle_out;

	WAVEHDR header;

	unsigned char *buf;
	DWORD buf_playing_cnt;
	DWORD buf_pending_cnt;
	DWORD buf_size;
};


static CALLBACK void waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
					DWORD dwParma1, DWORD dwParam2)
{
	struct pcm_sound_t *snd = (struct pcm_sound_t*)dwInstance;

	if (uMsg == WOM_DONE && snd != NULL) {
		EnterCriticalSection(&snd->locker);

		if (snd->buf_pending_cnt > 0) {
			memmove(snd->buf, snd->buf + snd->buf_playing_cnt, snd->buf_pending_cnt);
			snd->header.dwBufferLength = snd->buf_playing_cnt = snd->buf_pending_cnt;
			snd->buf_pending_cnt = 0;

			LeaveCriticalSection(&snd->locker);

			waveOutWrite(snd->handle_out, &snd->header, sizeof(WAVEHDR));
			return;
		} else {
			snd->buf_playing_cnt = 0;
		}

		LeaveCriticalSection(&snd->locker);
	}
}
static CALLBACK void waveInProc(HWAVEIN hwo, UINT uMsg, DWORD dwInstance,
					DWORD dwParma1, DWORD dwParam2)
{
	struct pcm_sound_t *snd = (struct pcm_sound_t*)dwInstance;
	int temp = snd->buf_size - snd->buf_playing_cnt;
	if (snd->buf_playing_cnt < snd->buf_size){
		memcpy(snd->buf, snd->header.lpData, temp);
		snd->buf_playing_cnt += temp;
		waveInAddBuffer(snd->handle_in, &snd->header, sizeof(WAVEHDR));
	}

	return;
}

void* loadAudioBlock(const char*filename, DWORD* blockSize)
 {
	HANDLE hFile= INVALID_HANDLE_VALUE;
 	DWORD size = 0;
 	DWORD readBytes = 0;
 	void* block = NULL;

 	if((hFile = CreateFile(
		filename,
		GENERIC_READ,
 		FILE_SHARE_READ,
	 	NULL,
 		OPEN_EXISTING,
 		0,
 		NULL
 	)) == INVALID_HANDLE_VALUE)

	return NULL;

	do {
		if((size = GetFileSize(hFile, NULL)) ==0)
		break;

		if((block = HeapAlloc(GetProcessHeap(),0, size)) == NULL)
		break;

		ReadFile(hFile, block, size,&readBytes, NULL);
	} while(0);
 	CloseHandle(hFile);
 	*blockSize = size;
 	return block;
}

int pcm_sound_open(struct sound_device *snd_dev, int type)
{
	WAVEFORMATEX wfx;
	struct pcm_sound_t *snd;

	if (snd_dev->priv != NULL ||
	    waveOutGetNumDevs() == 0 ||
	    snd_dev->channels <= 0 ||
	    snd_dev->bits_per_sample <= 0 ||
	    snd_dev->bits_per_sample % 8 != 0 ||
	    snd_dev->samples_per_sec <= 0) return -1;

	snd = (struct pcm_sound_t*)malloc(sizeof(struct pcm_sound_t));
	if (snd == NULL) return -1;

	memset(snd, 0, sizeof(struct pcm_sound_t));

	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = snd_dev->channels;
	wfx.wBitsPerSample = snd_dev->bits_per_sample;
	wfx.nBlockAlign = ((wfx.nChannels * wfx.wBitsPerSample) >> 3);
	wfx.nSamplesPerSec = snd_dev->samples_per_sec;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
	
	if (type == WRITE_SOUND){
		if (waveOutOpen(&snd->handle_out, WAVE_MAPPER, &wfx,
				(DWORD_PTR)waveOutProc, (DWORD_PTR)snd,
				CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
			printf("%s: waveOutOpen() failed.\n", __FUNCTION__);
			snd->handle_out = NULL;
			goto error;
		}
	}else if (type == READ_SOUND){
		if (waveInOpen(&snd->handle_in, WAVE_MAPPER, &wfx,
				(DWORD_PTR)waveInProc, (DWORD_PTR)snd,
				CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
			printf("%s: waveInOpen() failed.\n", __FUNCTION__);
			snd->handle_in = NULL;
			goto error;	
		}
	}else {
		printf("In %s, wave sound type %x error.\n", __func__, type);
		goto error;
	}

	snd->buf_size = 0x100000;
	if ((snd->buf = (unsigned char*)malloc((size_t)snd->buf_size)) == NULL) {
		printf("%s: allocate memory failed.\n", __FUNCTION__);
		goto error;
	}

	snd->buf_playing_cnt = 0;
	snd->buf_pending_cnt = 0;
	snd->header.lpData = snd->buf;
	snd->header.dwBufferLength = 0;
	snd->header.dwFlags = 0;
	snd->header.dwLoops = 0;
/*
	if (waveOutPrepareHeader(snd->handle_out, &snd->header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		printf("%s: waveOutPrepareHeader() failed.\n", __FUNCTION__);
		goto error;
	}
*/
	InitializeCriticalSection(&snd->locker);
	goto exit;

error:
	if (snd->handle_out != NULL) waveOutClose(snd->handle_out);
	if (snd->buf != NULL) free(snd->buf);
	free(snd);
	return -1;

exit:
	snd_dev->priv = (void*)snd;
	return 0;
}


int pcm_sound_close(struct sound_device *snd_dev)
{
	struct pcm_sound_t *snd = (struct pcm_sound_t*)snd_dev->priv;

	if (snd == NULL) return -1;

	waveOutReset(snd->handle_out);
	waveOutUnprepareHeader(snd->handle_out, &snd->header, sizeof(WAVEHDR));
	waveOutClose(snd->handle_out);
	free(snd->buf);
	DeleteCriticalSection(&snd->locker);
	free(snd);

	snd_dev->priv = NULL;

	return 0;
}


int pcm_sound_update(struct sound_device *snd_dev)
{
	/* TODO */
	return 0;
}


int pcm_sound_read(struct sound_device *snd_dev, void *buf, size_t count)
{
	if (-1 == pcm_sound_open(snd_dev, READ_SOUND)){
		return -1;	
	}

	struct pcm_sound_t *snd = (struct pcm_sound_t*)snd_dev->priv;
	snd->buf_size = count;
	snd->buf_playing_cnt = 0;
	if (snd == NULL || buf == NULL || count <= 0) return -1;

	EnterCriticalSection(&snd->locker);
	snd->header.lpData = buf;
	snd->header.dwBufferLength = count;

	waveInPrepareHeader(snd->handle_in, &snd->header, sizeof(WAVEHDR));
	waveInAddBuffer(snd->handle_in, &snd->header, sizeof(WAVEHDR));

	//record
//	printf("Start to Record...\n");
	waveInStart(snd->handle_in);
	
	while (snd->buf_playing_cnt < count){
		Sleep(1);
	}
	memcpy(buf, snd->buf, count);
//	printf("Record Over!\n");

	waveInStop(snd->handle_in);
	waveInReset(snd->handle_in);

	waveInUnprepareHeader(snd->handle_in, &snd->header, sizeof(WAVEHDR));
	waveInClose(snd->handle_in);
	free(snd->buf);
	DeleteCriticalSection(&snd->locker);
	free(snd);

	snd_dev->priv = NULL;

	LeaveCriticalSection(&snd->locker);
	return 0;
}


int pcm_sound_write(struct sound_device *snd_dev, void *buf, size_t count)
{
	if (-1 == pcm_sound_open(snd_dev, WRITE_SOUND)){
		return -1;	
	}

	struct pcm_sound_t *snd = (struct pcm_sound_t*)snd_dev->priv;
	
	if (snd == NULL || buf == NULL || count <= 0) return -1;

	EnterCriticalSection(&snd->locker);
#if 1
	if (count > 0) {
		unsigned char *tmp = snd->buf;
		memcpy(tmp, buf, count);
		snd->header.dwBufferLength = count;
		snd->header.lpData = snd->buf;
		LeaveCriticalSection(&snd->locker);
		if (waveOutPrepareHeader(snd->handle_out, &snd->header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
			printf("%s: waveOutPrepareHeader() failed.\n", __FUNCTION__);
			if (snd->handle_out != NULL) waveOutClose(snd->handle_out);
			return -1;
		}
		waveOutWrite(snd->handle_out, &snd->header, sizeof(WAVEHDR));
		waveOutUnprepareHeader(snd->handle_out, &snd->header, sizeof(WAVEHDR));

		pcm_sound_close(snd_dev);
		return 0;
	}
#endif
#if 0
	//test .wav music 
	void* block = NULL;
	DWORD blockSize;
	if((block = loadAudioBlock("c:\\temp\\ding.wav", &blockSize)) == NULL) {
		fprintf(stderr, "Unable to loadfile\n");
		return -1;
	}
	snd->header.dwBufferLength = blockSize;
	snd->header.lpData = block;
	LeaveCriticalSection(&snd->locker);
	if (waveOutPrepareHeader(snd->handle_out, &snd->header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
		printf("%s: waveOutPrepareHeader() failed.\n", __FUNCTION__);
		if (snd->handle_out != NULL) waveOutClose(snd->handle_out);
		return -1;
	}
	waveOutWrite(snd->handle_out, &snd->header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(snd->handle_out, &snd->header, sizeof(WAVEHDR));
	pcm_sound_close(snd_dev);
	return 0;

#endif

	LeaveCriticalSection(&snd->locker);
	pcm_sound_close(snd_dev);

	return 0;
}

#else /* !(defined(__MINGW32__) || defined(__CYGWIN__)) */

/* TODO */

int pcm_sound_open(struct sound_device *snd_dev, int type)
{
	return -1;
}


int pcm_sound_close(struct sound_device *snd_dev)
{
	return 0;
}


int pcm_sound_update(struct sound_device *snd_dev)
{
	return 0;
}


int pcm_sound_read(struct sound_device *snd_dev, void *buf, size_t count)
{
	return -1;
}


int pcm_sound_write(struct sound_device *snd_dev, void *buf, size_t count)
{
	return -1;
}

#endif /* defined(__MINGW32__) || defined(__CYGWIN__) */

