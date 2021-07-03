#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <vector>
#include <tuple>
#include <memory>

using std::string;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::vector;
using std::tuple;
using std::tie;
using std::make_tuple;
using std::shared_ptr;
using std::make_shared;


#include "geometry.h"




tuple<int, vector<shared_ptr<IObject>>, Image> repair_mechanism(Image& in, const string name);
int lb(const Matrix<uint>&b,Matrix<uint>&l);//сегментации объектов
void fll(const Matrix<uint>&b, Matrix<uint>&l, int i, int j, int c);//сегментация объекта 
void bnr(const Image &in, Matrix<uint>&bn);//функция бинаризации изображения
int csfr(const vector<shared_ptr<IObject>> &obj, Image& in, int res, int n,const string name);


int main(int argc, char **argv)
{
    
    if (argc != 4)
    {
        cout << "Usage: " << endl << argv[0]
             << " <in_image.bmp> <out_image.bmp> <out_result.txt>" << endl;
        return 0;
    }

    try {
        Image src_image = load_image(argv[1]);
        ofstream fout(argv[3]);
        vector<shared_ptr<IObject>> object_array;
        Image dst_image=src_image;
        int result_idx;
        tie(result_idx, object_array, dst_image) = repair_mechanism(dst_image, argv[2]);
        save_image(dst_image, argv[2]);
        fout << result_idx << endl;
        fout << object_array.size() << endl;
        for (const auto &obj : object_array)
            obj->Write(fout);

    } catch (const string &s) {
        cerr << "Error: " << s << endl;
        return 1;
    }
}




void variate(vector<string> &names,const int n)// возвращает названия картинок
{
  int len=names[0].length();
  string nm=names[0].substr(0,len-4);
  for(int i=1; i<=n; ++i)
  {
   char s='0'+i;
   string n1="data/pic/"+nm+"_"+s+".bmp";
   names.push_back(n1);
  }
}

int csfr(const vector<shared_ptr<IObject>> &obj, Image& in, int res, int n,const string name)
//n количество объектов
//res индекс объекта,которого нужно заменить
{
  int x0,y0; tie(x0,y0)=obj[res]->location;//
  int R=1000000, r=1000000;
  for(int i=0; i<n; ++i)
  {
    if(i==res) continue;
    int x,y; tie(x,y)=obj[i]->location;

    float distanse=sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0));
    float rmin,Rmax;
    tie(rmin,Rmax)=obj[i]->get();
    float R1=distanse-rmin, r1=distanse-Rmax;
    if(r1<r) r=r1;
    if(R1<R) R=R1;
  }//вычислила границы min_r и max_r
  //cout<<r<<" "<<R<<endl;
  vector<string> names;
  names.push_back(name);
  variate(names,3);
 // int k=3;
  for(int k=1; k<=3; ++k)
  {
    char const* nm=names[k].c_str();
    Image im=load_image(nm);
    int n_r=im.n_rows,n_c=im.n_cols;
    //border br; br.up=br.left=0; br.down=n_r-1; br.right=n_c-1;
    Matrix<uint> b(n_r,n_c);//+
    bnr(im, b);

//поиск центра
    int xc=0,yc=0, A=0; 
    for(int i=0;i<=n_r-1; ++i)
      for(int j=0; j<=n_c-1; ++j)
      {
        
        if(b(i,j)==1)
        {
          xc+=j; yc+=i; ++A;
        }
      }
     xc/=A; yc/=A;


///происходит проверка варианта, а потом вставит первый подходящий вариант
    bool st1=false, st2=false, st3=true;
    if(xc+r>=n_c||yc+r>=n_r) continue;
    if(xc<r||yc<r) continue;

    for(int a=0; a<360&&st3; ++a)//движение по окружности
    {
       int x1=xc+aroundInt((R)*cos(getAngle(a)));
       int y1=yc+aroundInt((R)*sin(getAngle(a)));

      if(x1<0||y1<0) continue;
      if(x1>=n_c||y1>=n_r) continue;
       int x2=xc+aroundInt((r)*cos(getAngle(a)));
       int y2=yc+aroundInt((r)*sin(getAngle(a)));
       if(!st1) if(b(y2,x2)==0) st1=true;
       if(!st2) if(b(y2,x2)==1) st2=true;
       if(b(y1,x1)==1) st3=false;
    }


   //cout<<"["<<k<<", "<<st1<<", "<<st2<<", "<<st3<<"]"<<endl;
   if(!(st1&&st2&&st3)) continue; 
   //cout<<k<<endl; 

//происходит вставка картины 
  for(int i=0;i<n_r;++i) for(int j=0; j<n_c;++j) if(b(i,j)) in(i+y0-yc,j+x0-xc)=im(i,j);
   

 
   return k;
  }
  //cout<<r<<" "<<R<<endl;


  return -1;//если ни один вариант не подошел
}

tuple<int, vector<shared_ptr<IObject>>, Image>
repair_mechanism(Image& in,const string name)
{
    int n_r=in.n_rows, n_c=in.n_cols;
    Matrix<uint> binImg(n_r,n_c),lblImg(n_r,n_c);

    auto object_array = vector<shared_ptr<IObject>>();

    int result_idx = 0;
  
    //процесс бинаризации
    bnr(in, binImg);//!

    //создание массива для сегментации объектов
    for(int i=0;i<n_r;++i)
      for(int j=0;j<n_c;++j)
        lblImg(i,j)=0;

    //процесс сегментации
    int nobj=lb(binImg,lblImg);

    //вычисление геометрических характеристик 
    result_idx=gemetry(lblImg,nobj,object_array);//геометрия

    //изменить картину 
    result_idx=csfr(object_array,in,result_idx, nobj, name);//изменения картины

    //создать кортеж и вернуть индекс, геометрические характеристики и картинку 
    return make_tuple(result_idx, object_array, in);
}

//сегментации объектов
int lb(const Matrix<uint>&b,Matrix<uint>&l)
{
  int n_r=b.n_rows, n_c=b.n_cols,c=0;
  for(int i=0; i<n_r; ++i)
    for(int j=0; j<n_c; ++j)
      if(b(i,j)&&!l(i,j)) fll(b,l,i,j,++c);
  return c;
}

//сегментация объекта 
void fll(const Matrix<uint>&b, Matrix<uint>&l, int i, int j, int c)
{
  int n_r=b.n_rows, n_c=b.n_cols;
  if(i<0||i==n_r||j<0||j==n_c) return;
  if(l(i,j)||!b(i,j)) return;
  l(i,j)=c;
  fll(b,l,i-1,j,c); 
  fll(b,l,i+1,j,c); 
  fll(b,l,i,j-1,c); 
  fll(b,l,i,j+1,c);
} 

//функция бинаризации изображения
void bnr(const Image &in, Matrix<uint>&bn)//ok
{
  int n_r=in.n_rows, n_c=in.n_cols;
  uint r,g,b,h=180;
  
  for(int i=0;i<n_r;++i)
    for(int j=0; j<n_c;++j)
    {
      tie(r,g,b)=in(i,j);
      if(r>h||g>h||b>h){ bn(i,j)=1;/* in(i,j)=make_tuple(0,180,0);*/}
      else {bn(i,j)=0;  /*in(i,j)=make_tuple(255,255,255);*/}
         
    }  
   
}



