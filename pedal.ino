#include <LiquidCrystal.h>
#include <SD.h>  
#include <TMRpcm.h> 
#include <SPI.h>  

int musicPin=9;
int SD_ChipSelectPin=10; //CS

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
TMRpcm tmrpcm;
 int nKicks=0, nSnares=0, selectedKick=0, selectedSnare=0;
 int kickButton=A5, snareButton=A4;
 int switchKickButton=A2, switchSnareButton=A3;
 int loudness=4;


File root;
char kicks[10][12]={{"0"}};
char snares[10][12]={{"0"}};

void setup() {
  Serial.begin(9600);
  //pinMode(musicPin, OUTPUT);
  pinMode(kickButton, INPUT_PULLUP);
  pinMode(snareButton, INPUT_PULLUP);
  pinMode(switchKickButton, INPUT_PULLUP);
  pinMode(switchSnareButton, INPUT_PULLUP);
  pinMode(musicPin, OUTPUT);
  tmrpcm.speakerPin=musicPin;
    lcd.begin(16,2);
  
    if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println("SD fail");  
                   lcd.setCursor(5,0);
           lcd.print("SD FAIL.");
    return;   // don't do anything more if not

  } else {
           Serial.println("SD ok");
         }


    root = SD.open("/");
  printDirectory(root, 0);

  Serial.println(nKicks);
  Serial.println(nSnares);
  
  for(int i=0; i<nKicks;i++)
  Serial.println(kicks[i]);
  
  for(int i=0; i<nSnares;i++)
  Serial.println(snares[i]);
  
}

void loop() {
updateVisual();
checkButtons();

}


void updateVisual()
{
  lcd.setCursor(0,0);
  lcd.print(kicks[selectedKick]);
  
  lcd.setCursor(0,1);
  lcd.print(snares[selectedSnare]);

  lcd.setCursor(15,0);
  lcd.print(loudness);
}


void checkButtons()
{
  if(digitalRead(kickButton)==0)
  {
  tmrpcm.play(kicks[selectedKick]);
  delay(100);
  tmrpcm.disable();
  while(digitalRead(kickButton)==0)
    if(digitalRead(snareButton)==0) 
    {
      loudness=(loudness+1)%7;
      while(digitalRead(snareButton)==0);
    }
  }

  if(digitalRead(snareButton)==0)
  {
    delay(100);
  tmrpcm.disable();
    tmrpcm.play(snares[selectedSnare]);
    while(digitalRead(snareButton)==0);
  }

  //loudness
  tmrpcm.setVolume(loudness);


    if(digitalRead(switchSnareButton)==0)
  {
    selectedSnare=(selectedSnare+1)%nSnares;
    while(digitalRead(switchSnareButton)==0);
  }

      if(digitalRead(switchKickButton)==0)
  {
    selectedKick=(selectedKick+1)%nKicks;
    while(digitalRead(switchKickButton)==0);
  }

if(!tmrpcm.isPlaying()) tmrpcm.disable();



}






















void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
     // Serial.print('\t');
    }
    //Serial.print(entry.name());
    if (entry.isDirectory()) {
      //Serial.println("/");
      //printDirectory(entry, numTabs + 1);
      ;
    } else {
      // files have sizes, directories do not
     // Serial.print(entry.name());

      //String fileName(entry.name());
      char fileName[10]={'0'};
      strcpy(fileName,entry.name());
      
      //Serial.println(fileName);
      
      if(strstr(fileName,"KICK"))
        {
          //Serial.println(fileName);
          strcpy(kicks[nKicks],fileName);
          nKicks++;
        }

      if(strstr(fileName,"SNARE"))
        {
          strcpy(snares[nSnares],fileName);
          //Serial.println(snares[nSnares]);
          nSnares++;
        }
        
    //  Serial.print("\t\t");
    //  Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
