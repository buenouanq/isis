/* Linux ALSA interface library for Isis
   stefan agamanolis
   october 2000
*/

#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

#include <linux/videodev.h>

#include <isis.h>
#include "isis_v4l2.h"

#define STREAMBUFS 4
#define MAXSTANDARDS 25

/**************************************************************************/
/* function prototypes */

static Value *isis_v4l2_capture_open(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv);
static Value *isis_v4l2_capture_close(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv);
static Value *isis_v4l2_capture_input(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv);
static Value *isis_v4l2_capture_format(Script *script, char *proc_name, int call_data,
				       int argc, Value **argv);
static Value *isis_v4l2_capture_period(Script *script, char *proc_name, int call_data,
				       int argc, Value **argv);
static Value *isis_v4l2_capture_standards(Script *script, char *proc_name, int call_data,
					  int argc, Value **argv);
static Value *isis_v4l2_capture_standard(Script *script, char *proc_name, int call_data,
					 int argc, Value **argv);
static Value *isis_v4l2_capture_start(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv);
static Value *isis_v4l2_capture_read(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv);
static Value *isis_v4l2_capture_queue(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv);
static Value *isis_v4l2_capture_stop(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv);
static Value *isis_v4l2_find_control(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv);
static Value *isis_v4l2_control(Script *script, char *proc_name, int call_data,
				int argc, Value **argv);
static Value *isis_v4l2_tuner(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv);
static Value *isis_v4l2_freq(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv);
static Value *isis_v4l2_tune(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv);

/**************************************************************************/
/* structures */

typedef struct bufinfo {
  struct v4l2_buffer vidbuf;
  char *data;
  int length;
} bufinfo;

typedef struct capinfo {
  int fd;
  int curbuf;
  struct bufinfo bufs[STREAMBUFS];
  struct v4l2_format format;
  struct v4l2_requestbuffers req;
} capinfo;

/**************************************************************************/
/* bindings */

/* --------------------------------------------------------------------- */
void bind_v4l2_library(Script *script)
{
  bindBool(script, "isis_v4l2", 1);
  
  bindInt(script, "vcap-rgb32", V4L2_PIX_FMT_RGB32);
  bindInt(script, "vcap-bgr32", V4L2_PIX_FMT_BGR32);
  bindInt(script, "vcap-rgb24", V4L2_PIX_FMT_RGB24);
  bindInt(script, "vcap-bgr24", V4L2_PIX_FMT_BGR24);
  bindInt(script, "vcap-rgb332", V4L2_PIX_FMT_RGB332);
  bindInt(script, "vcap-rgb555", V4L2_PIX_FMT_RGB555);
  bindInt(script, "vcap-rgb565", V4L2_PIX_FMT_RGB565);
  bindInt(script, "vcap-grey", V4L2_PIX_FMT_GREY);
  bindInt(script, "vcap-yuv410", V4L2_PIX_FMT_YUV410);
  bindInt(script, "vcap-yuv420", V4L2_PIX_FMT_YUV420);
  bindInt(script, "vcap-yuyv", V4L2_PIX_FMT_YUYV);
  bindInt(script, "vcap-uyvy", V4L2_PIX_FMT_UYVY);

  bindInt(script, "vcap-field-interlaced", V4L2_FIELD_INTERLACED);
  bindInt(script, "vcap-field-top", V4L2_FIELD_TOP);
  bindInt(script, "vcap-field-bottom", V4L2_FIELD_BOTTOM);
  bindInt(script, "vcap-field-both", V4L2_FIELD_ALTERNATE);
  bindInt(script, "vcap-field-any", V4L2_FIELD_ANY);

  bindInt(script, "vcap-audio-mono", V4L2_TUNER_MODE_MONO);
  bindInt(script, "vcap-audio-stereo", V4L2_TUNER_MODE_STEREO);
  bindInt(script, "vcap-audio-lang1", V4L2_TUNER_MODE_LANG1);
  bindInt(script, "vcap-audio-lang2", V4L2_TUNER_MODE_LANG2);
  bindInt(script, "vcap-audio-sap", V4L2_TUNER_MODE_SAP);

  bindCfunc(script, "vcap-open", isis_v4l2_capture_open, 0);
  bindCfunc(script, "vcap-close", isis_v4l2_capture_close, 0);
  bindCfunc(script, "vcap-input", isis_v4l2_capture_input, 0);
  bindCfunc(script, "vcap-format", isis_v4l2_capture_format, 0);
  bindCfunc(script, "vcap-period", isis_v4l2_capture_period, 0);
  bindCfunc(script, "vcap-standards", isis_v4l2_capture_standards, 0);
  bindCfunc(script, "vcap-standard", isis_v4l2_capture_standard, 0);
  bindCfunc(script, "vcap-start", isis_v4l2_capture_start, 0);
  bindCfunc(script, "vcap-read", isis_v4l2_capture_read, 0);
  bindCfunc(script, "vcap-queue", isis_v4l2_capture_queue, 0);
  bindCfunc(script, "vcap-stop", isis_v4l2_capture_stop, 0);
  bindCfunc(script, "vcap-find-control", isis_v4l2_find_control, 0);
  bindCfunc(script, "vcap-control", isis_v4l2_control, 0);
  bindCfunc(script, "vcap-tuner", isis_v4l2_tuner, 0);
  bindCfunc(script, "vcap-freq", isis_v4l2_freq, 0);
  bindCfunc(script, "vcap-tune", isis_v4l2_tune, 0);

}

/**************************************************************************/
/* function definitions */

/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_open(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv)
{
  struct capinfo *info;
  
  checkCount(1, "capture device port");
  checkList(0, "capture device port");
  
  info = malloc( sizeof(struct capinfo) );  
  info->fd = getInt(Listitem(argv[0], PORT_INTERNAL_ID));

  return newAddr(info);
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_close(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv)
{
  struct capinfo *info;
  
  checkCount(1, "capture handle");
  checkAddr(0, "capture handle");
  
  info = getAddr(argv[0]);
  free(info);
  
  return NullValue;
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_input(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv)
{
  struct capinfo *info;
  int input;
  
  checkCount(2, "capture handle, input number");
  checkAddr(0, "capture handle");
  checkInt(1, "input number");   
  
  info = getAddr(argv[0]);
  input = getInt(argv[1]);
  
  if( ioctl(info->fd, VIDIOC_S_INPUT, &input) ) perror("* V4L2 set input failed");
  
  if( ioctl(info->fd, VIDIOC_G_INPUT, &input) ) {
    perror("* V4L2 get input failed");
    return NullValue;  
  }

  return newInt(input);

}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_format(Script *script, char *proc_name, int call_data,
				       int argc, Value **argv)
{
  struct capinfo *info;
  int width, height, pixelformat, fieldmode;
  Value *sizeval, *formatval, *fieldval, *retval;
  
  checkCount(4, "capture handle, framesize, format, field mode");
  checkAddr(0, "capture handle");
  checkIntList(1, "framesize");   
  checkInt(2, "format");   
  checkInt(3, "field mode");   
  
  info = getAddr(argv[0]);
  width = getInt(Listitem(argv[1], 0));
  height = getInt(Listitem(argv[1], 1));
  pixelformat = getInt(argv[2]);
  fieldmode = getInt(argv[3]);
 
  info->format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  
  /* get current format */

  if( ioctl(info->fd, VIDIOC_G_FMT, &info->format) ) {
    perror("* V4L2 get format failed");
    return NullValue;
  }

  /* request new format */

  info->format.fmt.pix.width = width;
  info->format.fmt.pix.height = height;
  info->format.fmt.pix.pixelformat = pixelformat;
  info->format.fmt.pix.field = fieldmode;

  if( ioctl(info->fd, VIDIOC_S_FMT, &info->format) ) {
    perror("* V4L2 set format failed");
    return NullValue;
  }

  /* return actual format */

  sizeval = newIntListva(2, info->format.fmt.pix.width, info->format.fmt.pix.height);
  formatval = newInt(info->format.fmt.pix.pixelformat);
  fieldval = newInt(info->format.fmt.pix.field);

  retval = newListva(3, sizeval, formatval, fieldval);
  
  freeValue(sizeval);
  freeValue(formatval);
  freeValue(fieldval);

  return retval;
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_period(Script *script, char *proc_name, int call_data,
				       int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_streamparm streamparm;
  double fper;
  
  checkCountLeast(1, "capture handle, { frame period }");

  checkAddr(0, "capture handle");
  info = getAddr(argv[0]);
  streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

  if(argc > 1) {
    
    checkNumber(1, "frame period");   
    fper = getRealfromNumber(argv[1]);
    
    if( ioctl(info->fd, VIDIOC_G_PARM, &streamparm) ) {
      perror("* V4L2 get capture parm failed");
      return NullValue;
    }
    
    if( !(streamparm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME) ) {
      perror("* V4L2 time per frame not supported on this card");
      return NullValue;
    }

    streamparm.parm.capture.timeperframe.numerator = (int) (fper * 10000000.0);
    streamparm.parm.capture.timeperframe.denominator = 10000000;
    
    if( ioctl(info->fd, VIDIOC_S_PARM, &streamparm) ) {
      perror("* V4L2 set capture parm failed");
      return NullValue;
    }
  }
  
  if( ioctl(info->fd, VIDIOC_G_PARM, &streamparm) ) {
    perror("* V4L2 get capture parm failed");
    return NullValue;
  }
  
  if (streamparm.parm.capture.timeperframe.numerator == 0) return NullValue;
  return newReal( ((double)(streamparm.parm.capture.timeperframe.numerator)) /
		  ((double)(streamparm.parm.capture.timeperframe.denominator)) );
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_standards(Script *script, char *proc_name, int call_data,
					  int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_standard standard;
  struct v4l2_input input;
  int count, i;
  char names[MAXSTANDARDS][25];
  Value *stringvals[MAXSTANDARDS];
  Value *retval;

  checkCount(2, "capture handle, input number");
  checkAddr(0, "capture handle");
  checkInt(1, "input number");
  
  info = getAddr(argv[0]);
  input.index = getInt(argv[1]);
  
  if( ioctl(info->fd, VIDIOC_ENUMINPUT, &input) ) {
    perror("* V4L2 enum input failed");
    return NullValue;
  }
  
  count = 0;
  for(i = 0; i < MAXSTANDARDS ; i++) {
    standard.index = i;
    if( ioctl(info->fd, VIDIOC_ENUMSTD, &standard) ) break;
    if( standard.id & input.std ) strcpy(names[count++], standard.name);
  }
  
  for(i = 0; i < count; i++) stringvals[i] = newString(strlen(names[i]), names[i]);
  retval = newList(count, stringvals);
  for(i = 0; i < count; i++) freeValue(stringvals[i]);
  
  return retval;
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_standard(Script *script, char *proc_name, int call_data,
					 int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_standard standard;
  char name[25], upname[25];
  int i, namelen;

  checkCount(2, "capture handle, standard");
  checkAddr(0, "capture handle");
  checkString(1, "standard");
  
  info = getAddr(argv[0]);
  getString(argv[1], name, 24);
  strcpy(upname, name);
  namelen = strlen(upname);
  for(i = 0; i < namelen; i++) upname[i] = toupper(upname[i]);

  for(i = 0; ; i++) {
    standard.index = i;
    if( ioctl(info->fd, VIDIOC_ENUMSTD, &standard) ) {
      perror("* V4L2 standard not recognized");
      return NullValue;
    }
    if( strcmp(name, standard.name) == 0 ) break;
    if( strcmp(upname, standard.name) == 0 ) break;
  }
      
  if( ioctl(info->fd, VIDIOC_S_STD, &standard.id) ) {
    perror("* V4L2 set standard failed");
    return NullValue;
  }
  
  return newBool(1);
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_start(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv)
{
  struct capinfo *info;
  int numbufs, i;
  struct v4l2_buffer *vidbuf;
  void **bufaddrs;
  Value *retval;

  checkCount(1, "capture handle");
  checkAddr(0, "capture handle");
  
  info = getAddr(argv[0]);

  if( ioctl(info->fd, VIDIOC_G_FMT, &info->format) ) return NullValue;
  
  /* request buffers */

  info->req.count = STREAMBUFS;
  info->req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  info->req.memory = V4L2_MEMORY_MMAP;
  
  if( ioctl(info->fd, VIDIOC_REQBUFS, &info->req) ) return NullValue;
  
  /* query and map buffers */
  
  numbufs = info->req.count;
  for(i = 0; i < numbufs; i++) {
    vidbuf = &info->bufs[i].vidbuf;
    vidbuf->index = i;
    vidbuf->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if( ioctl(info->fd, VIDIOC_QUERYBUF, vidbuf) ) {
      perror("* V4L2 querybuf failed");
      return NullValue;
    }
    info->bufs[i].length = 0;
    info->bufs[i].data = mmap(0, vidbuf->length, PROT_READ | PROT_WRITE,
			      MAP_SHARED, info->fd, vidbuf->m.offset);
    if(info->bufs[i].data == (void *) -1) {
      perror("* V4L2 mmap failed");
      return NullValue;
    }
    info->bufs[i].length = vidbuf->length;
    if( ioctl(info->fd, VIDIOC_QBUF, vidbuf) ) {
      perror("* V4L2 qbuf failed");
      return NullValue;
    }
  }
  
  /* turn on stream */

  if( ioctl(info->fd, VIDIOC_STREAMON, &info->bufs[0].vidbuf.type) ) {
    perror("* V4L2 streamon failed");
    return NullValue;
  }

  info->curbuf = -1;

  /* return buffer addresses */

  bufaddrs = malloc(numbufs * sizeof(void *));
  for(i = 0; i < numbufs; i++) bufaddrs[i] = info->bufs[i].data;
  retval = newAddrList(numbufs, bufaddrs);
  free(bufaddrs);

  return retval;
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_read(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_buffer tempbuf;

  checkCount(1, "capture handle");
  checkAddr(0, "capture handle");
  
  info = getAddr(argv[0]);
  
  if(info->curbuf >= 0) {
    if( ioctl(info->fd, VIDIOC_QBUF, &info->bufs[info->curbuf].vidbuf) ) {
      perror("* V4L2 qbuf failed");
      return NullValue;
    }
    info->curbuf = -1;
  }

  tempbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if( ioctl(info->fd, VIDIOC_DQBUF, &tempbuf) ) {
    perror("* V4L2 dqbuf failed");
    return NullValue;
  }
  
  info->curbuf = tempbuf.index;

  return newBasicListva(6, 
			IsisInt, (int) tempbuf.index,
			IsisAddr, (void *) info->bufs[info->curbuf].data,
			IsisInt, (int) tempbuf.bytesused,
			IsisBool, (int) (tempbuf.field & V4L2_FIELD_TOP),
			IsisInt, tempbuf.timestamp.tv_sec,
			IsisInt, tempbuf.timestamp.tv_usec);
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_queue(Script *script, char *proc_name, int call_data,
				      int argc, Value **argv)
{
  struct capinfo *info;

  checkCount(1, "capture handle");
  checkAddr(0, "capture handle");
  
  info = getAddr(argv[0]);
  
  if(info->curbuf >= 0) {
    if( ioctl(info->fd, VIDIOC_QBUF, &info->bufs[info->curbuf].vidbuf) ) {
      perror("* V4L2 qbuf failed");
      return NullValue;
    }
    info->curbuf = -1;
  }
  
  return newBool(1);
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_capture_stop(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv)
{
  struct capinfo *info;
  int i;
  
  checkCount(1, "capture handle");
  checkAddr(0, "capture handle");
  
  info = getAddr(argv[0]);
  
  i = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if( ioctl(info->fd, VIDIOC_STREAMOFF, &i) ) {
    perror("* V4L2 streamoff failed");
    return NullValue;
  }
  
  for(i = 0; i < info->req.count; i++) {
    if(info->bufs[i].data) munmap(info->bufs[i].data, info->bufs[i].length);
    info->bufs[i].data = NULL;
  }
  
  return newBool(1);
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_find_control(Script *script, char *proc_name, int call_data,
				     int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_queryctrl queryctrl;
  char name[32];
  int i, namelen, gotit = 0;
  
  checkCount(2, "capture handle, control name");
  checkAddr(0, "capture handle");
  checkString(1, "control name");

  info = getAddr(argv[0]);
  getString(argv[1], name, 31);
  
  for(i = 0; i < 100 && !gotit ; i++) {
    queryctrl.id = V4L2_CID_BASE + i;
    if( ioctl(info->fd, VIDIOC_QUERYCTRL, &queryctrl) ) continue;
    if( strcmp(name, queryctrl.name) == 0 ) gotit = 1;
  }
  
  if(!gotit) {
    for(i = 0; i < 100 && !gotit ; i++) {
      queryctrl.id = V4L2_CID_PRIVATE_BASE + i;
      if( ioctl(info->fd, VIDIOC_QUERYCTRL, &queryctrl) ) continue;
      if( strcmp(name, queryctrl.name) == 0 ) gotit = 1;
    }
  }

  if(!gotit) {
    perror("* V4L2 control not found");
    return NullValue;
  }
  
  return newBasicListva(5, IsisInt, queryctrl.id, 
			IsisInt, queryctrl.minimum, IsisInt, queryctrl.maximum,
			IsisInt, queryctrl.step, IsisInt, queryctrl.default_value);
}


/* --------------------------------------------------------------------- */
static Value *isis_v4l2_control(Script *script, char *proc_name, int call_data,
				int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_control control;
  
  checkCountLeast(2, "capture handle, control id, { value }");
  checkAddr(0, "capture handle");
  checkInt(1, "control id");

  info = getAddr(argv[0]);
  control.id = getInt(argv[1]);

  if (argc > 2) {
    checkInt(2, "value");
    control.value = getInt(argv[2]);
    if( ioctl(info->fd, VIDIOC_S_CTRL, &control) ) {
      perror("* V4L2 could not set control value");
      return NullValue;
    }
  }

  if( ioctl(info->fd, VIDIOC_G_CTRL, &control) ) {
    perror("* V4L2 could not get control value");
    return NullValue;
  }

  return newInt(control.value);
}

/* --------------------------------------------------------------------- */
static Value *isis_v4l2_tuner(Script *script, char *proc_name, int call_data,
			      int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_tuner tuner;
  struct v4l2_input input;
  Value *nameval, *stdval, *lowval, *highval, *afcval, *audval, *sigval, *retval;
  
  checkCountLeast(2, "capture handle, input number, { audio mode }");
  checkAddr(0, "capture handle");
  checkInt(1, "input number");
  if (argc > 2) checkInt(2, "audio mode");
  
  info = getAddr(argv[0]);
  input.index = getInt(argv[1]);
  
  if( ioctl(info->fd, VIDIOC_ENUMINPUT, &input) ) {
    perror("* V4L2 enum input failed");
    return NullValue;
  }

  if (argc > 2) {
    tuner.index = input.tuner;

    if( ioctl(info->fd, VIDIOC_G_TUNER, &tuner) ) {
      perror("* V4L2 get tuner failed");
      return NullValue;
    }  

    tuner.audmode = getInt(argv[2]);

    if( ioctl(info->fd, VIDIOC_S_TUNER, &tuner) ) {
      perror("* V4L2 set tuner failed");
      return NullValue;
    }
  }
  
  tuner.index = input.tuner;

  if( ioctl(info->fd, VIDIOC_G_TUNER, &tuner) ) {
    perror("* V4L2 get tuner failed");
    return NullValue;
  }
  
  nameval = newString(strlen(tuner.name), tuner.name);
  sigval = newInt(tuner.signal);
  lowval = newInt(tuner.rangelow);
  highval = newInt(tuner.rangehigh);
  afcval = newInt(tuner.afc);
  audval = newInt(tuner.audmode);
  
  retval = newListva(6, nameval, sigval, lowval, highval, afcval, audval);
  
  freeValue(nameval);
  freeValue(sigval);
  freeValue(lowval);
  freeValue(highval);
  freeValue(afcval);
  freeValue(audval);
  
  return retval;
}

/* --------------------------------------------------------------------- */
static Value *isis_v4l2_freq(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_frequency freq;
  struct v4l2_input input;
  
  checkCount(2, "capture handle, input number");
  checkAddr(0, "capture handle");
  checkInt(1, "input number");   
  
  info = getAddr(argv[0]);
  input.index = getInt(argv[1]);
  
  if( ioctl(info->fd, VIDIOC_ENUMINPUT, &input) ) {
    perror("* V4L2 enum input failed");
    return NullValue;
  }
  
  freq.tuner = input.tuner;
  freq.type = V4L2_TUNER_ANALOG_TV;
  
  if( ioctl(info->fd, VIDIOC_G_FREQUENCY, &freq) ) {
    perror("* V4L2 get frequency failed");
    return NullValue;
  }
  
  return newInt(freq.frequency);
}

/* --------------------------------------------------------------------- */
static Value *isis_v4l2_tune(Script *script, char *proc_name, int call_data,
			     int argc, Value **argv)
{
  struct capinfo *info;
  struct v4l2_frequency freq;
  struct v4l2_input input;

  checkCount(3, "capture handle, input number, frequency");
  checkAddr(0, "capture handle");
  checkInt(1, "input number");   
  checkInt(2, "frequency");   
  
  info = getAddr(argv[0]);
  input.index = getInt(argv[1]);
  
  if( ioctl(info->fd, VIDIOC_ENUMINPUT, &input) ) {
    perror("* V4L2 enum input failed");
    return NullValue;
  }

  freq.tuner = input.tuner;
  freq.type = V4L2_TUNER_ANALOG_TV;
  freq.frequency = getInt(argv[2]);
  
  if( ioctl(info->fd, VIDIOC_S_FREQUENCY, &freq) ) {
    perror("* V4L2 set frequency failed");
    return NullValue;
  }
  
  return newInt(freq.frequency);
}

