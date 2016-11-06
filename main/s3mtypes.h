#ifndef general_h_included
#define general_h_included 1

#ifdef __WATCOMC__
#include <stdbool.h>
#include <stdint.h>
#endif

typedef struct channel_t {
  // general switches :
    uint8_t  bEnabled;      // flag if =0 then nothing to mix at the moment
    uint8_t  bChannelType;  // 0=off,1=left,2=right,3,4=adlib ... if 0,3,4 -> everything ignored !
  // current Instrument :
    uint16_t InstrSEG;      // DOS segment of current instrument data
    uint16_t SampleSEG;     // DOS segment of current sample data
    uint8_t  InstrNo;       // number of instrument is currently playing
    uint8_t  Note;          // we don't need it really for playing, but let's store it anyway
  // copy of sampledata (maybe it differs a bit):
    uint8_t  bSampleVol;    // current sample volume
    uint8_t  sLoopflag;     // flag if we have to loop sample
    uint16_t sSmpstart;     // start offset of sample
    uint16_t sLoopstart;    // loop start of current sample =0ffffh if no loop
    uint16_t sLoopend;      // loop end/normal end of current sample
    uint32_t sCurPos;       // fixed point value for current position in sample
    uint32_t sStep;         // fixed point value of frequency step (distance of one step
                            // depends on period we play currently)
    uint16_t sPeriod;       // st3 period ... you know these amiga values (look at tech.doc of ST3)
                            // period does no influence playing a sample direct, but it's for sliding etc.
    uint16_t lower_border;  // B-7 or B-5 period for current instrument to check limits
    uint16_t upper_border;  // C-0 or C-3 period for current instrument to check limits
  // effect info :
    uint16_t command;       // 2 times effectnumber (for using a jmptable)
    uint16_t cmd2nd;        // 2nd command part - for multiple effects
    uint8_t  bParameter;    // just the command parameters
  // extra effect data :
    uint8_t  continueEf;    // Flag if we should continue effect - vibrato,tremolo
    uint16_t VibtabOfs;     // for each channel its own choise (default = OFS sinuswave)
    uint16_t TrmtabOfs;     // Offset of wavetable for tremolo
    uint8_t  tablepos;      // <-- we reset this if a effect starts uses such a table
    uint8_t  VibPara;       // <-- for dual command Vib + Vol
    uint8_t  PortPara;      // <-- for dual command Port + Vol
    uint16_t OldPeriod;     // save that value for some effects
    uint8_t  Oldvolume;     // save that value for tremolo
    uint16_t WantedPeri;    // <- period to slide to with Portamento
    uint8_t  ArpegPos;      // which of thoses 3 notes we currently play ...
    uint8_t  note1;         //--.
    uint8_t  note2;         //---- +note - 3 notes we do arpeggio between
    uint32_t Step0;         //--.
    uint32_t Step1;         //  |- the 3 step values we switch between in arpeggio effect (0 is start value <- we have to refesh after arpeggio)
    uint32_t Step2;         //--'
    uint8_t  cTick;         //---- ticks left to retrigg note
    uint8_t  savNote;       //--.
    uint8_t  savInst;       //  | - new values for notedelay ...
    uint8_t  SavVol;        //  |
    uint8_t  ndTick;        //--' <- also used for Notecut (ticks left to cut)
};

typedef struct instrument_t {
    uint8_t  bType;         // 1: instrument
    uint8_t  DOSname[12];
    uint8_t  unused1;
    uint16_t memseg;        // segment of sampledata
    uint16_t slength;       // length of sample <= 64KB
    uint16_t HI_length;     // <- not used !
    uint16_t loopbeg;       // loop begin <= 64KB
    uint16_t HI_loopbeg;    // <- not used !
    uint16_t loopend;       // loop end <= 64KB
    uint16_t HI_loopend;    // <- not used !
    uint8_t  vol;           // default instrument volume
    uint8_t  unused2;
    uint8_t  packinfo;      // =0 or we can't play it !
    uint8_t  flags;         // bit 0: loop (all other bits are ignored)
    uint16_t c2speed;       // frequency at middle C (actually C-4)
    uint16_t c2speed_hi;    // ignored
    uint8_t  unused3[12];
    uint8_t  IName[28];
    uint32_t SCRS_ID;
};

#endif
