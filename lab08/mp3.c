/*
 * mp3.c
 *
 *  Created on:   Aug 13, 2018
 *  Modififed on: Nov 28, 2018
 *           -- add call to getID3Tags() to ensure ID3 headers are skipped
 *              prior to start of decoding
 *
 *      Author: khughes
 *
 * MP3 handler code.  Derived from libmad file minimad.c
 */

/*
 * libmad - MPEG audio decoder library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: minimad.c,v 1.4 2004/01/23 09:41:32 rob Exp $
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Information needed for MP3 code
#include "mad.h"

// Peripherals
#include "timer2A.h"

// FatFs
#include "ff.h"

// Other project files
#include "control.h"
#include "sound.h"
#include "ID3.h"
#include "MP3utils.h"

// Storage used by MP3 decoder; this seems to be the smallest size which
// works reliably.
static uint8_t inputBuffer[5*512];

/*
 * This is perhaps the simplest example use of the MAD high-level API.
 * Standard input is mapped into memory via mmap(), then the high-level API
 * is invoked with three callbacks: input, output, and error. The output
 * callback converts MAD's high-resolution PCM samples to 16 bits, then
 * outputs them in little-endian, stereo-interleaved format.
 */

/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */

struct buffer {
  bool started; // Flag used by output() to determine when to configure
                //    Timer2A for the DAC.
  FIL *fp;      // file pointer
};

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

static
enum mad_flow input(void *data,
		    struct mad_stream *stream)
{
  struct buffer *buffer = data;
  uint32_t remaining = 0;
  uint8_t *bufptr = inputBuffer;
  UINT br;

  // if paused, this is a good a place as any to twiddle our thumbs.
  while( isPaused() );

  // check to see if any frames remain to be processed
  if( stream->next_frame != NULL ) {
    remaining = stream->bufend - stream->next_frame;
    memmove( inputBuffer, stream->next_frame, remaining );
    bufptr += remaining;
  } 

  // Read a block into the other end of buffer
  f_read( buffer->fp, bufptr, 512, &br );
  decipher( bufptr, 512 );

  // If there's no more data or we're changing songs, we're done
  if( br == 0 || isDone() ) {
    return MAD_FLOW_STOP;
  } else {
    bufptr += br;
    remaining += br;
    mad_stream_buffer( stream, inputBuffer, remaining );
    return MAD_FLOW_CONTINUE;
  }
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */

static
enum mad_flow output(void *data,
		     struct mad_header const *header,
		     struct mad_pcm *pcm)
{
  unsigned int nchannels;
  struct buffer *buffer = data;
  register mad_fixed_t const *left_ch, *right_ch;
  // Note: pcm->samplerate contains the sampling frequency of the MP3 file

  nchannels = pcm->channels;
  left_ch   = pcm->samples[0];

  // If the file is mono, duplicate the left channel
  if( nchannels == 1 )
    right_ch = left_ch;
  else
    right_ch  = pcm->samples[1];

  // If this is the start of the data, set up the DAC timer
  if( buffer->started == false ) {
    setTimer2ARate( header->samplerate );
    enableTimer2A( true );
    buffer->started = true;
  }

  // Copy data to the output buffers
  fillBuffers( (uint32_t *)left_ch, (uint32_t *)right_ch, pcm->length );

  return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */

int MP3decode( FIL *fp ) {
  struct buffer buffer;
  struct mad_decoder decoder;
  int result;

  /* Be sure to position MP3 stream past any ID3 header, if any */

  getID3Tags( fp, NULL );

  /* initialize the private message structure */

  buffer.fp = fp;
  buffer.started = false;

  /* configure input, output, and error functions */

  mad_decoder_init(&decoder, &buffer,
		   input,
               0 /* header */,
               0 /* filter */,
           output,
               0 /* error */,
               0 /* message */);

  /* start decoding */

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  /* release the decoder */

  mad_decoder_finish(&decoder);

  f_close( fp );

  return result;
}
