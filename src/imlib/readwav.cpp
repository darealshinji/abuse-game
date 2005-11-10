#include "readwav.hpp"
#include "specs.hpp"
#include "macs.hpp"
#include "dprint.hpp"

struct wav_chunk
{
  char id[4];
  unsigned long size;
  char type[4];
} ;

struct wav_tag
{
  char id[4];
  unsigned long size;
} ;


struct wav_format
{
  unsigned short fmt_tag,channels;
  unsigned long samplesps,avg_bytesps;
  unsigned short align;
} ;


struct pcm_wave
{
  wav_format wf;
  unsigned short bitsps;
} ;




void read_chunk(wav_chunk &chunk, bFILE *fp)
{
  fp->read(&chunk.id,4);
  chunk.size=fp->read_uint32();
  fp->read(&chunk.type,4);  
}

void read_tag(wav_tag &tag, bFILE *fp)
{
  fp->read(&tag.id,4);
  tag.size=fp->read_uint32();
}

void read_wav_format(wav_format &fmt, bFILE *fp)
{
  fmt.fmt_tag=fp->read_uint16();
  fmt.channels=fp->read_uint16(); 
  fmt.samplesps=fp->read_uint32();
  fmt.avg_bytesps=fp->read_uint32();  
  fmt.align=fp->read_uint16();  
}


void read_pcm(pcm_wave &pcm, bFILE *fp)
{
  read_wav_format(pcm.wf,fp);
  pcm.bitsps=fp->read_uint16();  
}



void write_wav(char *filename, long sample_rate, long data_size, unsigned char *data)
{
  bFILE *fp=open_file(filename,"wb");
  if (fp->open_failure())
  {
    printf("Unable to open %s for writing\n", filename);
    delete fp;
    exit(1);
  }

  /***************  Write the chunk ***************************/
  fp->write((void *)"RIFF",4);  
  fp->write_uint32(data_size+36);
  fp->write((void *)"WAVE",4);


  /************** Write the tag *******************************/
  fp->write((void *)"fmt ",4);
  fp->write_uint32(16);
  
  
  /************** Write PCM ***********************************/
  fp->write_uint16(1);          // format_tag
  fp->write_uint16(1);          // mono recording
  fp->write_uint32(sample_rate);
  fp->write_uint32(sample_rate);   // average bytes per sec
  fp->write_uint16(1);    // allignment? Don't know what this does?
  fp->write_uint16(8);    // 8 bits per sample
  
  /************* Write data tag ******************************/
  fp->write((void *)"data",4);
  fp->write_uint32(data_size);

  /************ Now write sample data ************************/
  fp->write(data,data_size);

  delete fp;
}



unsigned char *read_wav(char *filename, long &sample_rate, long &data_size)
{
  unsigned char *data;
  wav_chunk chunk;
  wav_tag tag;
  pcm_wave pcm;

  bFILE *fp=open_file(filename,"rb");
  if (fp->open_failure())
  { delete fp; return NULL; }
  read_chunk(chunk,fp);      
  if (memcmp(chunk.type,"WAVE",4)!=0)
  {
    printf("Bad WAV file (chunk) %s\n",filename);
    delete fp;
    return NULL;    
  }
  
  read_tag(tag,fp);
  if (memcmp(tag.id,"fmt ",4)!=0)
  {
    printf( "fmt tag missing, bad file (%s)\n",filename);
    delete fp;
    return NULL;
  }


  read_pcm(pcm,fp);

  fp->seek(tag.size-16,SEEK_CUR);  // seek to offset of sample

  read_tag(tag,fp);

  if (memcmp(tag.id,"data",4)!=0)
  {
    printf("Bad Wav file (tag), %s\n",filename);
    delete fp;
    return NULL;
  }
  
  data_size=tag.size;  
  data=(unsigned char *)jmalloc(tag.size,"WAV data");
  ERROR(data,"Malloc error");

  sample_rate=pcm.wf.samplesps;     
  ERROR((unsigned int)fp->read(data,tag.size)==tag.size,"Premature end of file");
  ERROR(pcm.bitsps==8,"Only 8-bit samples supported");
  ERROR(pcm.wf.channels==1,"Only mono samples supported");  
  ERROR(pcm.wf.align==1,"Bad block allignment");   
  delete fp;
  return data;
}



