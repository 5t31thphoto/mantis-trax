#include "ui.h"
#include <M5Cardputer.h>
static constexpr uint16_t C_BG=0x0000,C_GRID=0x52AA,C_TXT=0xFFFF,C_GREEN=0x07E0,C_RED=0xF800,C_YELLOW=0xFFE0,C_CYAN=0x07FF,C_GRAY=0x7BEF;
static void mantis(int x,int y,int s){auto&d=M5Cardputer.Display;d.fillCircle(x,y,s/3,C_GREEN);d.drawLine(x-s,y,x-s/3,y-s/3,C_GREEN);d.drawLine(x+s,y,x+s/3,y-s/3,C_GREEN);d.drawLine(x-s/3,y+s/2,x-s,y+s,C_GREEN);d.drawLine(x+s/3,y+s/2,x+s,y+s,C_GREEN);d.drawLine(x-s/2,y-s/2,x-s*2/3,y-s,C_GREEN);d.drawLine(x+s/2,y-s/2,x+s*2/3,y-s,C_GREEN);}
void uiBegin(){M5Cardputer.Display.setRotation(1);M5Cardputer.Display.setTextFont(0);M5Cardputer.Display.setTextSize(1);M5Cardputer.Display.fillScreen(C_BG);}
void uiSplash(){auto&d=M5Cardputer.Display;d.fillScreen(C_BG);mantis(120,48,26);d.setTextColor(C_GREEN);d.setTextDatum(middle_center);d.setTextSize(2);d.drawString("MantisTrax",120,88);d.setTextSize(1);d.setTextColor(C_GRAY);d.drawString("OFF-GRID TRACKER LOCATOR",120,111);delay(1400);}
static String maskKey(const String&s){if(s.length()<8)return "----------------";return s.substring(0,4)+"..."+s.substring(s.length()-4);}
static void drawSetup(const AppConfig&c,int field,bool editing,const String&edit){auto&d=M5Cardputer.Display;d.fillScreen(C_BG);d.setTextDatum(top_left);d.setTextColor(C_GREEN);d.drawString("MANTISTRAX SETUP",6,4);d.setTextColor(C_GRAY);d.drawString("ENTER=edit/confirm  FN+BACKSPACE=back",6,17);const char* names[]={"DevAddr","NwkSKey","AppSKey","DR","SAVE"};for(int i=0;i<5;i++){int y=34+i*18;d.setTextColor(i==field?C_GREEN:C_TXT);d.drawString(names[i],6,y);String v;if(i==0)v=editing?edit:c.devAddr;else if(i==1)v=editing?edit:maskKey(c.nwkSKey);else if(i==2)v=editing?edit:maskKey(c.appSKey);else if(i==3)v=String(c.preferredDr);else v="[ CONFIRM ]";d.drawString(v,72,y);}}
bool uiSetup(AppConfig&c){
  int field=0;
  bool editing=false;
  String edit;
  uint32_t redraw=0;
  for(;;){
    M5Cardputer.update();
    auto k=M5Cardputer.Keyboard.keysState();
    if(M5Cardputer.Keyboard.isChange()&&M5Cardputer.Keyboard.isPressed()){
      if(k.fn && k.del){
        if(editing){ editing=false; edit=""; }
        else return false;
      } else if(editing){
        if(k.fn){
          // Fn-layer navigation/escape is represented by the base character
          // in older M5Cardputer library versions.
          for(char ch:k.word){
            if(ch==',' || ch=='.'){
              // Left/right are not useful while entering fixed-width hex.
              continue;
            }
            if(ch=='`') { editing=false; edit=""; }
          }
        } else {
          for(char ch:k.word){
            if(isxdigit((unsigned char)ch) && edit.length()<32)
              edit+=(char)toupper((unsigned char)ch);
          }
        }
        if(k.del && !k.fn){
          if(edit.length()) edit.remove(edit.length()-1);
        }
        if(k.enter){
          if(field==0 && edit.length()==8) c.devAddr=edit;
          else if(field==1 && edit.length()==32) c.nwkSKey=edit;
          else if(field==2 && edit.length()==32) c.appSKey=edit;
          else if(field==3 && edit.length()) c.preferredDr=(uint8_t)constrain(edit.toInt(),0L,3L);
          editing=false;
        }
      } else {
        if(k.fn){
          for(char ch:k.word){
            if(ch==';') field=(field+4)%5;
            else if(ch=='.') field=(field+1)%5;
            else if(ch=='`') return false;
          }
        } else {
          if(k.del) return false;
          if(k.enter){
            if(field<4){
              editing=true;
              edit=(field==0?c.devAddr:(field==1?c.nwkSKey:(field==2?c.appSKey:String(c.preferredDr))));
            } else {
              configSave(c);
              return true;
            }
          }
        }
      }
    }
    if(millis()-redraw>50){drawSetup(c,field,editing,edit);redraw=millis();}
    delay(1);
  }
}
void uiRadar(const AppConfig&c,const LocalFix&l,const TrackerFix&t,const TargetVector&v,float heading,const RadioStatus&rs){auto&d=M5Cardputer.Display;d.fillScreen(C_BG);int w=d.width(),h=d.height(),cx=w/2,cy=h/2+8;d.setTextDatum(top_left);d.setTextSize(1);d.setTextColor(C_GREEN);d.drawString("MANTISTRAX",4,2);d.setTextColor(C_GRAY);d.drawString(l.valid?"GPS FIX":"GPS SEARCH",78,2);d.drawString(rs.locked?"LOCK":"SCAN",190,2);int r=48;d.drawCircle(cx,cy,r,C_GRID);d.drawCircle(cx,cy,r/2,C_GRID);d.drawLine(cx-r-8,cy,cx+r+8,cy,C_GRID);d.drawLine(cx,cy-r-8,cx,cy+r+8,C_GRID);
 float hr=(heading-90)*M_PI/180.0f;int hx=cx+(int)(24*cos(hr)),hy=cy+(int)(24*sin(hr));d.drawLine(cx,cy,hx,hy,C_RED);d.fillTriangle(hx,hy,hx-4,hy+7,hx+4,hy+7,C_RED);
 if(v.valid){float maxR=max(50.0f,c.maxRangeM);float q=min(1.0f,(float)(v.distance_m/maxR));float a=(v.relative_deg-90)*M_PI/180.0f;int tx=cx+(int)(q*r*cos(a)),ty=cy+(int)(q*r*sin(a));for(int rr=6;rr>=2;rr-=2)d.drawCircle(tx,ty,rr,((millis()/250)%2)?C_GREEN:C_CYAN);}
 d.setTextColor(C_TXT);String dist="--";if(v.valid){double x=c.units=="imperial"?v.distance_m*3.28084:v.distance_m;dist=x<5280?String(x,0)+(c.units=="imperial"?" ft":" m"):String(x/5280.0,2)+" mi";}d.drawString(dist,4,118);d.drawString(v.valid?String(v.bearing_deg,0)+"°":"--",78,118);d.drawString("HDG "+String(heading,0),130,118);d.drawString("BAT "+String(t.battery)+"%",195,118);d.setTextColor(C_GRAY);d.drawString("target "+String(rs.packets)+"  RSSI "+String(rs.rssi),4,104);}
