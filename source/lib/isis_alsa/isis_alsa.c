/* Linux ALSA interface library for Isis
   stefan agamanolis
   october 2000
*/

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <stdio.h>
#include <isis.h>
#include <alsa/asoundlib.h>
#include "isis_alsa.h"

/**************************************************************************/
/* function prototypes */

static Value *isis_alsa_pcm_open(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_close(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_filedes(Script *script, char *proc_name, int call_data,
				    int argc, Value **argv);
static Value *isis_alsa_pcm_setup(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_prepare(Script *script, char *proc_name, int call_data,
				    int argc, Value **argv);
static Value *isis_alsa_pcm_start(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_pause(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_reset(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_drop(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv);
static Value *isis_alsa_pcm_drain(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_state(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_delay(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_avail(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);
static Value *isis_alsa_pcm_link(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv);
static Value *isis_alsa_pcm_unlink(Script *script, char *proc_name, int call_data,
				   int argc, Value **argv);
static Value *isis_alsa_pcm_read(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv);
static Value *isis_alsa_pcm_write(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv);

/**************************************************************************/
/* bindings */

/* --------------------------------------------------------------------- */
void bind_alsa_library(Script *script)
{
  bindBool(script, "isis_alsa", 1);
  
  bindCfunc(script, "alsa-open-output", isis_alsa_pcm_open, 1);
  bindCfunc(script, "alsa-open-input", isis_alsa_pcm_open, 0);
  bindCfunc(script, "alsa-close", isis_alsa_pcm_close, 0);

  bindCfunc(script, "alsa-filedes", isis_alsa_pcm_filedes, 0);
  bindCfunc(script, "alsa-setup", isis_alsa_pcm_setup, 0);
  bindCfunc(script, "alsa-prepare", isis_alsa_pcm_prepare, 0);
  bindCfunc(script, "alsa-start", isis_alsa_pcm_start, 0);
  bindCfunc(script, "alsa-pause", isis_alsa_pcm_pause, 1);
  bindCfunc(script, "alsa-resume", isis_alsa_pcm_pause, 0);
  bindCfunc(script, "alsa-reset", isis_alsa_pcm_reset, 0);
  bindCfunc(script, "alsa-drop", isis_alsa_pcm_drop, 0);
  bindCfunc(script, "alsa-drain", isis_alsa_pcm_drain, 0);
  bindCfunc(script, "alsa-state", isis_alsa_pcm_state, 0);
  bindCfunc(script, "alsa-delay", isis_alsa_pcm_delay, 0);
  bindCfunc(script, "alsa-avail", isis_alsa_pcm_avail, 0);
  bindCfunc(script, "alsa-link", isis_alsa_pcm_link, 0);
  bindCfunc(script, "alsa-unlink", isis_alsa_pcm_unlink, 0);
  
  bindCfunc(script, "alsa-read", isis_alsa_pcm_read, 0);
  bindCfunc(script, "alsa-write", isis_alsa_pcm_write, 0);
}

/**************************************************************************/
/* helper function */

static void print_alsa_error(int code)
{
  fprintf(stderr, "Alsa error: %s\n", snd_strerror(code));
}

/**************************************************************************/
/* function definitions */

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_open(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv)
{
  int stream, ret, len;
  snd_pcm_t *pcm;
  char *str;

  if(argc > 0) {
    checkString(0, NULL);
    len = Listsize(argv[0]) + 1;
    str = malloc(len);
    getString(argv[0], str, len);
  }
  else str = strdup("plughw:0,0");
  
  if(call_data) stream = SND_PCM_STREAM_PLAYBACK;
  else stream = SND_PCM_STREAM_CAPTURE;
  
  ret = snd_pcm_open(&pcm, str, stream, 0);

  free(str);
  
  if (ret == 0) return newAddr(pcm);
  return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_close(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_close(getAddr(argv[0])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_filedes(Script *script, char *proc_name, int call_data,
				    int argc, Value **argv)
{
  struct pollfd ufds;
  
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if ( snd_pcm_poll_descriptors(getAddr(argv[0]), &ufds, 1) == 1 ) return newInt(ufds.fd);
  return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_setup(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  int bits, unsignd, bigendian, rate, channels;
  int dir, ret;
  snd_pcm_t *pcm;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;
  snd_pcm_format_t format;
  snd_pcm_uframes_t numsamps;

  checkCount(7, "handle, bits, unsigned_flag, big_endian_flag, rate, channels, numsamps");
  checkAddr(0, "handle");
  checkInt(1, "bits");
  checkBool(2, "unsigned_flag");
  checkBool(3, "big_endian_flag");
  checkInt(4, "rate");
  checkInt(5, "channels");
  checkInt(6, "numsamps");

  pcm = getAddr(argv[0]);
  bits = getInt(argv[1]);
  unsignd = getBool(argv[2]);
  bigendian = getBool(argv[3]);
  rate = getInt(argv[4]);
  channels = getInt(argv[5]);
  numsamps = getInt(argv[6]);
  
  format = snd_pcm_build_linear_format(bits, bits, unsignd, bigendian);
  dir = 0;

  /* set hardware parameters */

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_hw_params_any(pcm, hwparams);
  snd_pcm_hw_params_set_access(pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  snd_pcm_hw_params_set_format(pcm, hwparams, format);
  snd_pcm_hw_params_set_channels(pcm, hwparams, channels);
  snd_pcm_hw_params_set_rate(pcm, hwparams, rate, 0);
  snd_pcm_hw_params_set_periods(pcm, hwparams, 2, 0);
  snd_pcm_hw_params_set_period_size_near(pcm, hwparams, &numsamps, &dir);
  if( (ret = snd_pcm_hw_params(pcm, hwparams)) != 0 ) {
    print_alsa_error(ret);
    return NullValue;
  }
  
  snd_pcm_sw_params_alloca(&swparams);
  snd_pcm_sw_params_current(pcm, swparams);
  if( (ret = snd_pcm_sw_params(pcm, swparams)) != 0 ) {
    print_alsa_error(ret);
    return NullValue;
  }
  
  /* return actual period size to be used */
  return newInt(numsamps);
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_prepare(Script *script, char *proc_name, int call_data,
				    int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_prepare(getAddr(argv[0])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_start(Script *script, char *proc_name, int call_data,
			       int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_start(getAddr(argv[0])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_pause(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_pause(getAddr(argv[0]), call_data) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_reset(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_reset(getAddr(argv[0])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_drop(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_drop(getAddr(argv[0])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_drain(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_drain(getAddr(argv[0])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_state(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  snd_pcm_state_t state;

  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  state = snd_pcm_state(getAddr(argv[0]));
  
  return newBool( state == SND_PCM_STATE_RUNNING );
  
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_delay(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  snd_pcm_sframes_t delay;

  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_delay(getAddr(argv[0]), &delay) == 0 ) return newInt(delay);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_avail(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  snd_pcm_sframes_t avail;

  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( (avail = snd_pcm_avail_update(getAddr(argv[0]))) < 0 ) return NullValue;
  return newInt(avail);
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_link(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv)
{
  checkCount(2, "handle1, handle2");
  checkAddr(0, "handle1");
  checkAddr(1, "handle2");
  
  if( snd_pcm_link(getAddr(argv[0]), getAddr(argv[1])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_unlink(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv)
{
  checkCount(1, "handle");
  checkAddr(0, "handle");
  
  if( snd_pcm_unlink(getAddr(argv[0])) == 0 ) return newBool(1);
  else return NullValue;
}

/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_read(Script *script, char *proc_name, int call_data,
				 int argc, Value **argv)
{
  int ret;
  
  checkCount(3, "handle, numsamps, buffer");
  checkAddr(0, "handle");
  checkInt(1, "numsamps");
  checkAddr(2, "buffer");
  
  ret = snd_pcm_readi(getAddr(argv[0]), getAddr(argv[2]), getInt(argv[1]));
  
  if(ret > 0) return newInt(ret);
  return NullValue;
}


/* --------------------------------------------------------------------- */
static Value *isis_alsa_pcm_write(Script *script, char *proc_name, int call_data,
				  int argc, Value **argv)
{
  int ret;
  
  checkCount(3, "handle, numsamps, buffer");
  checkAddr(0, "handle");
  checkInt(1, "numsamps");
  checkAddr(2, "buffer");

  ret = snd_pcm_writei(getAddr(argv[0]), getAddr(argv[2]), getInt(argv[1]));

  if(ret > 0) return newInt(ret);
  return NullValue;
}


