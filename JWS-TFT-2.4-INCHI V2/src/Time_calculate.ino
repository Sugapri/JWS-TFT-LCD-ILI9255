#define d2r(x) x*M_PI/180
#define r2d(x) x*180/M_PI
const float lunarY          = 354.367068f;

void sholatCal(){
   DateTime now = rtc.now();
   float EJD = E_Julian_date(now.year(),now.month(),now.day(),lo);
   float Decl=Dql(EJD);
   float EqOfTime=EqT(EJD);
   Pray_Time(zona, la, lo,0,Decl, EqOfTime );
   toHijri(now.year(),now.month(),now.day()); 
 }
 
double E_Julian_date(int Year,int Month,int Days,double Longitude) // juliant date - 2451545
  {
    if (Month <= 2){
      Year -= 1;
      Month += 12;
    }
    double A = floor(((double)Year/100.0));
    double B = 2 - A + floor(A/4.0);
    double CLong = Longitude/(double)(15 * 24);
    double JD = floor(365.25 *(double)(Year+ 4716)) 
              - 2451545 
              + floor(30.6001 * (double)(Month + 1)) 
              + (double)Days + B 
              - 1524.5
              -CLong;
    return JD; 
  }


double EqT(const double EJD) 
  {
    double g = fix_angle(357.529f + 0.98560028f* EJD);
    double q = fix_angle(280.459f + 0.98564736f* EJD);
    double L = fix_angle(q + 1.915* sin(d2r(g)) + 0.020* sin(d2r(2*g)));
    double e = (23.439f - 0.00000036f* EJD);
    double RA = r2d(atan2(cos(d2r(e))* sin(d2r(L)), cos(d2r(L))))/ 15;
    double Eq =(q/15-fix_hour(RA));
    return Eq;
}
  
double Dql(double EJD) 
  {
    double g = fix_angle((357.529f + 0.98560028f* EJD));
    double q = fix_angle((280.459f + 0.98564736f* EJD));
    double L = fix_angle((q + 1.915f* sin(d2r(g)) + 0.020f* sin(d2r(2*g))));
    double e = (23.439f - 0.00000036f* EJD);
    double dd = r2d(asin(sin(d2r(e))* sin(d2r(L))));  // declination of the Sun
    return dd;
  }

double HourAngle( double Alfa, double Declination, double Latitude)
  {
     double rn =acos(
                      (-sin(d2r(Alfa))-sin(d2r(Latitude))*sin(d2r(Declination)))
                      /
                      (cos(d2r(Latitude))*cos(d2r(Declination)))
                     )/15;
     return r2d(rn);
  }


  void Pray_Time(float TimeZone, float Latitude, float Longitude,float Altitude,float Declination, float EquationOfTime )
  { 
    float BaseTime = fix_hour((12+TimeZone-(Longitude/15)-(EquationOfTime))) ;
    // Dzuhur
    sholatT[3] = BaseTime + iht;

    // Ashr
    float alfa =r2d(-atan(1 / (1+tan(d2r(fabs(Latitude-Declination))))));
    float HA = HourAngle(alfa,Declination,Latitude);
    sholatT[4] = BaseTime + HA + iht;

    // Maghrib
    alfa = 0.8333+0.0347*sqrt(Altitude);
    HA = HourAngle(alfa,Declination,Latitude);
    sholatT[5] = BaseTime + HA + iht;

    // Terbit
    sholatT[1] = BaseTime - HA;

    // Isya
    HA = HourAngle(18,Declination,Latitude);
    sholatT[6] = BaseTime + HA + iht;

    // Shubuh
    HA = HourAngle(20,Declination,Latitude);
    sholatT[0] = BaseTime - HA + iht;
      
    // Dhuha
    HA = HourAngle(-4.5,Declination,Latitude);
    sholatT[2] = BaseTime - HA;
  }

float fix_hour(float a)
  {
    a = a - 24.0 * floor(a / 24.0);
    a = a < 0.0 ? a + 24.0 : a;
    return a;
  }

float fix_angle(float a)
  {
    a = a - 360.0 * floor(a / 360.0);
    a = a < 0.0 ? a + 360.0 : a;
    return a;
  }  

//------------------------------------------
// Function calculate Hijriah Date
//------------------------------------------
long Days(uint16_t Y,uint8_t M,uint8_t D)
  {
    if (M < 3)
      { 
      Y -= 1; 
      M +=12;
      }
    Y = Y - 2000;
    long ndays= floor(365.25*Y)+floor(30.6001*(M + 1))+floor(Y/100)+floor(Y/400)+D+196;
    //long ndays= d1 + d2 - A + B + D + 196;
    return ndays;
  }

long DaysHijri(uint16_t Y,uint8_t M,uint8_t D)
  {
    Y = Y - 1420;   
    long hari = floor(29.5*M - 28.999)+floor(lunarY*Y)+D ;
    return hari;
  }
  
void toHijri(uint16_t Y, uint8_t M, uint8_t D)
  {
    long nday = Days(Y, M, D);
    if( floatnow > sholatT[5]) {nday++;} //Ganti tanggal baru hijriah setelah magrib
    long tahun = floor(nday/lunarY) + 1420;
    long bulan = 1;
    long harike = 1;
    while(DaysHijri(tahun, bulan, 1) <= nday){tahun++;};
    tahun--;
    while(DaysHijri(tahun, bulan, 1) <= nday){bulan++;};
    bulan--;
    harike = 1 + nday - DaysHijri(tahun, bulan, 1); //1
    if (bulan == 13){bulan = 12; harike += 29;};
    hD = harike;
    hM = bulan;
    hY = tahun;
  }  
