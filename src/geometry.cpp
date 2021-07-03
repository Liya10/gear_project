#include "geometry.h"


int gemetry(const Matrix<uint> &l, int n, vector<shared_ptr<IObject>> &results)
{
  int n_c=l.n_cols, n_r=l.n_rows;
  int result_idx=0;

  //каждый объект ограничим прямоугольником 
  vector <border> b(n);
  //сначала инициализируем 
  for(int i=0; i<n; ++i)
  {
    b[i].left=n_c-1; b[i].right=0;
    b[i].up=n_r-1; b[i].down=0;
  }
  //находим границы для каждого объекта
  for(int i=0; i<n_r; ++i)
    for(int j=0; j<n_c; ++j)
    {
      if(l(i,j)==0) continue;
      int num=l(i,j)-1;
      if(b[num].left>j) b[num].left=j;
      if(b[num].right<j) b[num].right=j;
      if(b[num].up>i) b[num].up=i;
      if(b[num].down<i) b[num].down=i;
    }


  //теперь для каждого объекта пытаемся вычислить геометрические характеристики  
  for(int num=1; num<=n; ++num)
  {
     int xc,yc;
     //вычисление центра, передаем границу, разметку, номер объекта
     tie(xc,yc)=getCentre(b[num-1],l,num);
     int R=getRadiusMax(b[num-1],l,xc,yc);
     int r=getRadiusMin(b[num-1],l,xc,yc);
     ///случай оси (не шестеренки)
     if(R==r)
     {
       result_idx=num-1;
       results.push_back(make_shared<Axis>(tuple<int,int>(xc,yc)));
     }

     //случай шестеренки
     else
     {
       int cogs;
       bool broken=false; 
       //количество зубчиков шестеренок 
       cogs=getCogs(b[num-1],l,xc,yc);
       // если кол-во отрицательное, значит шестеренка сломанная
       if(cogs<0){broken=true; cogs*=-1; result_idx=num-1;}

       results.push_back(make_shared<Gear>(tuple<int,int>(xc,yc),r,R,broken,cogs));
     }
   
   }
   return result_idx;
}


int aroundInt(float n)//ok
{
  int a=n;
  return (n-a>=a+1-n)?a+1:a;
}

tuple<int,int> getCentre(const border b, const Matrix<uint> &l, int num)
{  
    float x=0,y=0;
    int A=0;

    for(int i=b.up;i<=b.down; ++i)
      for(int j=b.left; j<=b.right; ++j)
      {
        int n2=l(i,j);
        if(n2==num)
        {
          x+=j; y+=i; ++A;
        }
      }
     x/=A; y/=A;//центр масс
    
    int xc=aroundInt(x),yc=aroundInt(y);
    int r=getRadiusMin(b,l,xc,yc) ;
    bool state=true;
    while(state)//ищет геометрический цетр
    {
      int rL=getRadiusMin(b,l,xc-1,yc),rR=getRadiusMin(b,l,xc+1,yc),rU=getRadiusMin(b,l,xc,yc-1),rD=getRadiusMin(b,l,xc,yc+1);
      int rLU=getRadiusMin(b,l,xc-1,yc-1),rRD=getRadiusMin(b,l,xc+1,yc+1),rRU=getRadiusMin(b,l,xc+1,yc-1),rLD=getRadiusMin(b,l,xc-1,yc+1);

      int xx,yy;

      if(rL>=rR){ xx=xc-1;} else{rL=rR; xx=xc+1;}
      if(rU>=rD){ yy=yc-1;} else{rU=rD; yy=yc+1;}
      if(rL>=rU){ yy=yc;} else{ rL=rU; xx=xc;}

      int x1,x2,y1,y2;
      if(rLU>=rRU){x1=xc-1; y1=yc-1; } else{rLU=rRU; x1=xc+1; y1=yc-1;}
      if(rLD>=rRD){x2=xc-1; y2=yc+1; } else{rLD=rRD;x2=xc+1; y2=yc+1; }
      if(rLU<rLD){ rLU=rLD;x1=x2; y1=y2; }

      if(rL<rLU){ rL=rLU; xx=x1; yy=y1;}

      if(r>=rL){ state=false;}
      else{r=rL; xc=xx;yc=yy; }
    }
    return make_tuple(xc,yc);
}


float getAngle(float n)
{
  return (n/180)*3.1415926535897;
}

int getRadiusMin(const border b, const Matrix<uint> &l, const int xc,const int yc)//OK
{
  int R=(b.right-b.left)/2+2; 
  for(int a=0; a<360; ++a)//движение по окружности
  {
     int x=xc+aroundInt(R*cos(getAngle(a)));
     int y=yc+aroundInt(R*sin(getAngle(a)));
     while(l(y,x)==0){--R; x=xc+aroundInt(R*cos(getAngle(a)));y=yc+aroundInt(R*sin(getAngle(a)));}
  }
  return R+1;
}
int getRadiusMax(const border b, const Matrix<uint> &l, const int xc,const int yc)//ok
{
  uint num=l(yc,xc);

  int R=(b.right-b.left)/2;
  int max=0;
  int x=xc+R;
  int y=yc; 
  while(l(y,x)==0){--R; x=xc+R;}
  for(int a=1; a<360; ++a)
  {
     x=xc+aroundInt(R*cos(getAngle(a)));
     y=yc+aroundInt(R*sin(getAngle(a)));
     while(l(y,x)==num)
     {
        int x1=xc+aroundInt((R+1)*cos(getAngle(a)));
        int y1=yc+aroundInt((R+1)*sin(getAngle(a)));
        if(y1<=b.down&&x1<=b.right&&y1>=b.up&&x1>=b.left) {if(l(y1,x1)==0) break;}
        else break;
        ++R; x=x1; y=y1;
     }

       if(R>max) max=R;
  }
  return max-1;
}
int getCogs(const border b, const Matrix<uint> &l, const int xc,const int yc)// считает количество зубьев
                                                                //вернет отрицательное, если она сломанная
{
  uint num=l(yc,xc);
  const int R=(getRadiusMax(b,l,xc,yc)+getRadiusMin(b,l,xc,yc))/2;
  int cogs=0;
  bool st1=false;//
  bool flag=false;// 
  bool broken=false;
  int fi=0;//угол между зубьями
  int sfi=0;
  for(int a=0; a<360; ++a)
  {
     int x=xc+aroundInt(R*cos(getAngle(a)));
     int y=yc+aroundInt(R*sin(getAngle(a)));

     if(l(y,x)==num&&!flag)//если наткнулись на зуб
     {
       ++cogs;flag=true;
       if(a==0){ st1=true; continue;}//чтобы не считал дважды
       if(!broken) // 
       { 
         int fi2=a-sfi;
        
         if(fi&&(abs(fi-fi2)>2)) broken=true;
         if(fi==0&&sfi) fi=fi2;
         sfi=a;
       }
     }
     else if(l(y,x)==0&&flag)//если мы вышли из зуба
     {
        flag=false;
        if(a==359) st1=false;
     }

  }
  if(st1) --cogs;
  if(broken) cogs*=-1;
  return cogs;
}
