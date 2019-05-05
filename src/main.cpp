#define SDFAT

#include <SPI.h>
#include <SdFat.h>

#include <TMRpcm.h>           //  also need to include this library...
// https://github.com/greiman/SdFat/blob/master/examples/DirectoryFunctions/DirectoryFunctions.ino


#define LDR_PIN A0
#define OPEN_TRESHOLD 600
#define CLOSED_TRESHOLD 300
#define BAK_OPEN_DIR "/bak/open"
#define BAK_CLOSED_DIR "/bak/closed"

int bak_status = 1;

char *ptrsOpenFiles[20];
int numOpenFiles = 0;
int currentOpenFile = 0;

char *ptrsClosedFiles[20];
int numClosedFiles = 0;
int currentClosedFile = 0;

void readOpenFiles();
void readClosedFiles();
void play_open();
void play_closed();

// SD default chip select pin.
const uint8_t chipSelect = 10;

// file system object
SdFat sd;

//------------------------------------------------------------------------------

 TMRpcm* audio;   // create an object for use in this sketch

void setup()
{
  Serial.begin(9600);

  // Wait for USB Serial
  while (!Serial) {
    SysCall::yield();
  }

  Serial.println("Type any character to start");
  while (!Serial.available()) {
    SysCall::yield();
  }

  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }

  readOpenFiles();
  readClosedFiles();

  audio = new TMRpcm;
  (*audio).speakerPin = 9; //5,6,11 or 46 on Mega, 9 on Uno, Nano, etc

}

void loop()
{
  delay(333);
  int sensorValue = analogRead(LDR_PIN);

  if( sensorValue < CLOSED_TRESHOLD && bak_status == 1)
  {
    bak_status = 0;
    play_closed();
  }

  if( sensorValue > OPEN_TRESHOLD && bak_status == 0)
  {
    bak_status = 1;
    play_open();
  }
}

void play_open()
{
  Serial.println("Opening ...");
  if(currentOpenFile >= numOpenFiles) currentOpenFile = 0;

  if((*audio).isPlaying())
  {
    Serial.println("skipping b/c still playing");
    return;
  }

  String filename = BAK_CLOSED_DIR;
  filename += "/";
  filename += ptrsOpenFiles[currentOpenFile++];
  Serial.println("playing " + filename);
  //(*audio).play(filename.c_str());

}

void play_closed()
{
  Serial.println("Closing ...");
  if(currentClosedFile == numClosedFiles) currentClosedFile = 0;

  if(0)//(*audio).isPlaying())
  {
    Serial.println("skipping b/c still playing");
    return;
  }

  String filename = BAK_CLOSED_DIR;
  filename += "/";
  filename += ptrsClosedFiles[currentClosedFile++];
  Serial.println("playing " + filename);
  //(*audio).play(filename.c_str());
}

void readFiles(const char* dir, char *files[], int *n)
{
  SdFile root;
  SdFile file;

  if (!root.open(dir)) {
    sd.errorHalt("open OPEN_DIR failed");
  }

  while (file.openNext(&root, O_RDONLY))
  {
    int ln;
    char fname[13];
    file.getName(fname,&ln);
    file.close();
    Serial.print(" > ");
    Serial.print(*n);
    Serial.print(" > ");
//    Serial.println((*files)[(*n)-1]);
    files[(*n)++] = strdup( fname);
    Serial.println(files[(*n)-1]);
  }
}

void readOpenFiles()
{
  //ptrsOpenFiles = (char**) malloc(20*13*sizeof(char));
  Serial.println("readOpenFiles()");
  readFiles(BAK_OPEN_DIR, ptrsOpenFiles, &numOpenFiles);
}


void readClosedFiles()
{
  //ptrsOpenFiles = (char**) malloc(20*13*sizeof(char));
  Serial.println("readClosedFiles()");
  readFiles(BAK_CLOSED_DIR, ptrsClosedFiles, &numClosedFiles);
}
