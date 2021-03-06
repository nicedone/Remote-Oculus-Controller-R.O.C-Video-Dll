#pragma once

#ifndef ROCSINK_H
#define ROCSINK_H

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <stdint.h>

extern "C" 
{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixfmt.h"
#include "libswscale/swscale.h"

#define INBUF_SIZE 4096
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

}

// Define a data sink (a subclass of "MediaSink") to receive the data for each subsession (i.e., each audio or video 'substream').
// In practice, this might be a class (or a chain of classes) that decodes and then renders the incoming audio or video.
// Or it might be a "FileSink", for outputting the received data into a file (as is done by the "openRTSP" application).

class ROCSink : public MediaSink {
public:
	static ROCSink* createNew(UsageEnvironment& env, MediaSubsession& subsession, int id , char const* streamId = NULL); // identifies the stream itself (optional)

private:
	ROCSink(UsageEnvironment& env, MediaSubsession& subsession, int id , char const* streamId);
	// called only by "createNew()"
	virtual ~ROCSink();

	static void afterGettingFrame(void* clientData, unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);
	void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes, struct timeval presentationTime, unsigned durationInMicroseconds);

private:
	// redefined virtual functions:
	virtual Boolean continuePlaying();

private:
	u_int8_t* fReceiveBuffer;
	u_int8_t* fReceiveBufferAV;
	MediaSubsession& fSubsession;
	char* fStreamId;

private: //H264
	u_int8_t const* sps;
	unsigned spsSize;
	u_int8_t const* pps;
	unsigned ppsSize;

public:	
	void setSprop(u_int8_t const* prop, unsigned size);

private: //FFMPEG
	AVCodec *codec;
	AVCodecContext *c;
	int frame;
	int got_picture;
	int len;
	AVFrame *picture;
	uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	char buf[1024];
	AVPacket avpkt;
	int id;
};


#endif
