#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <time.h>
#include <objidl.h>
#include <gdiplus.h>
#include <fstream>
#include "resource.h"
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#include <CommCtrl.h>
#pragma comment(lib,"comctl32")
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
char* toChar(TCHAR* str)
{
    int len=lstrlen(str);
    char* ret=new char[len+1];
    wcstombs(ret,str,len);
    return ret;
}
int atoi(TCHAR* str)
{
    char* t=toChar(str);
    int n=atoi(t);
    delete[] t;
    return n;
}
TCHAR* intToTchar(int number)
{
    TCHAR* res = new TCHAR[20];
    wsprintf(res, _T("%d"), number);
    return res;
}
static class fieldStyle
{
public:
    DWORD Visible;
    DWORD VerticalScroll;
    DWORD HorizontalScroll;
    DWORD TabStop;
    DWORD WantReturn;
    DWORD MultiLine;
    DWORD AutoVerticalScroll;
    DWORD AutoHorizontalScroll;
    DWORD AutoScroll;
    DWORD ReadOnly;
    DWORD Border;
    DWORD Caption;
    DWORD Disabled;
    DWORD Center;
    DWORD Left;
    DWORD Right;
    DWORD NumberOnly;
    DWORD Password;
public:
    fieldStyle()
    {
        Visible=WS_VISIBLE;
        VerticalScroll=WS_VSCROLL;
        HorizontalScroll=WS_HSCROLL;
        TabStop=WS_TABSTOP;
        WantReturn=ES_WANTRETURN;
        MultiLine=ES_MULTILINE;
        AutoHorizontalScroll=ES_AUTOHSCROLL;
        AutoVerticalScroll=ES_AUTOVSCROLL;
        AutoScroll=AutoHorizontalScroll | AutoVerticalScroll;
        ReadOnly=ES_READONLY;
        Border=WS_BORDER;
        Caption=WS_CAPTION;
        Disabled=WS_DISABLED;
        Center=ES_CENTER;
        Left=ES_LEFT;
        Right=ES_RIGHT;
        NumberOnly=ES_NUMBER;
        Password=ES_PASSWORD;

    }
};
const fieldStyle FieldStyle;
int windowCounter=5000;
int pow(int n,int x)
{
    int rez=1;
    for(int i=0;i<x;i++)
    {
        rez*=n;
    }
    return rez;
}
class Window
{
private:
    HWND hParent;
    HINSTANCE hInst;

public:
    HWND hWnd;
    int x,y;
    int width,height;
    DWORD styles;
    int dlgId;
    Window()
    {
        x=0;
        y=0;
        width=0;
        height=0;
        hWnd=0;
        hParent=0;
        hInst=0;
    }

    //============================================
    int getId()
    {
        this->dlgId=GetDlgCtrlID(hWnd);
        return this->dlgId;
    }
    void setWindow(HWND hWnd)
    {
        this->hWnd=hWnd;
    }
    TCHAR* getWindowText()
    {
        int maxLength=GetWindowTextLength(hWnd)+1;
        TCHAR* ret=new TCHAR[maxLength];
        GetWindowText(hWnd,ret,maxLength);
        return ret;
    }
    void updateFromWindow()
    {
        LPRECT t=new RECT;
        GetClientRect(hWnd,t);
        this->x=t->left;
        this->y=t->top;
        this->width=t->right-t->left;
        this->height=t->bottom-t->top;
    }
    void update()
    {
        UpdateWindow(hWnd);
    }
    LRESULT sendMessage(UINT message,WPARAM wParam=0,LPARAM lParam=0)
    {
        return SendMessage(hWnd,message,WPARAM(&wParam),LPARAM(&lParam));
    }
    LRESULT postMessage(UINT message,WPARAM wParam=0,LPARAM lParam=0)
    {
        return PostMessage(hWnd,message,WPARAM(&wParam),LPARAM(&lParam));
    }
    HWND getWindow()
    {
        return hWnd;
    }
    DWORD getStyle()
    {
        return styles;
    }
    void setStyle(const DWORD style)
    {
        styles=style;
        SetWindowLong(hWnd,GWL_STYLE,style);
        UpdateWindow(hWnd);
    }
    void setRead(bool readOnly)
    {
        if(readOnly)
        {
            addStyle(FieldStyle.ReadOnly);
        }
        else
        {
            removeStyle(FieldStyle.ReadOnly);
        }
    }
    void addStyle(DWORD style)
    {
        styles |= style;
        setStyle(styles);
    }
    void removeStyle(DWORD style)
    {
        styles &=~style;
        setStyle(styles);
    }
    void enableWindow(bool enable)
    {
        EnableWindow(hWnd,enable);
    }
    bool isStyleSet(DWORD style)
    {
        if((styles&~style)==styles)
            return false;
        else
            return true;
    }
    void setFocus(bool focus)
    {
        if(focus)
            SetFocus(hWnd);
        else
            SetFocus(GetParent(hWnd));
    }
    void setPosition(int x,int y)
    {
        this->x=x;
        this->y=y;
        MoveWindow(hWnd,x,y,this->width,this->height,true);
    }
    void setX(int x)
    {
        setPosition(x,this->y);
    }
    void setY(int y)
    {
        setPosition(this->x,y);
    }
    void setSize(int width,int height)
    {
        if(width<10) width=10;
        if(height<10) height=10;
        this->width=width;
        this->height=height;
        MoveWindow(hWnd,x,y,this->width,this->height,true);
    }
    void setWidth(int width)
    {
        setSize(width,this->height);
    }
    void setHeight(int height)
    {
        setSize(this->width,height);
    }
    void getPosition(int &x,int &y)
    {
        x=this->x;
        y=this->y;
    }
    int getX()
    {
        return x;
    }
    int getY()
    {
        return y;
    }
    void getSize(int &width,int &height)
    {
        width=this->width;
        height=this->height;
    }
    int getWidth()
    {
        return this->width;
    }
    int getHeight()
    {
        return this->height;
    }
    //============================================
};
class TextField: public Window
{
private:
    HWND hWnd;
    int x,y;
    int width,height;
    DWORD styles;
public:
    TextField(int x,int y,int width,int height,HWND parent,HINSTANCE hInst,DWORD defaultStyle)
    {
        this->x=x;
        this->y=y;
        if(width<10) width=10;
        if(height<10) height=10;
        this->width=width;
        this->height=height;
        styles=WS_CHILD | WS_VISIBLE | defaultStyle; 
        hWnd=CreateWindowEx(WS_EX_CLIENTEDGE,L"EDIT",0,styles,x,y,width,height,parent,NULL,hInst,0);
    }
    int innerLength() //возвращает длинну введенной строки
    {
        return GetWindowTextLength(hWnd);
    }

    TCHAR* getValue()
    {
        int maxLength=GetWindowTextLength(hWnd);
        TCHAR* ret=new TCHAR[maxLength];
        GetWindowText(hWnd,ret,maxLength);
        return ret;
    }
    void setValue(TCHAR* value)
    {
        SetWindowText(hWnd,value);    
    }
    void setValue(char* value)
    {
        TCHAR* temp=new TCHAR[strlen(value)];
        wsprintf(temp,L"%S",value);
        SetWindowText(hWnd,temp);
    }
    void clear()
    {
        SetWindowText(hWnd,NULL);
    }


    void setSelect(int start=-1,int end=-1)
    {
        if(end==-1) end=GetWindowTextLength(hWnd);
        if(start==-1) start=0;
        SendMessage(hWnd,EM_SETSEL,start,end);
    }
    void getSelect(int &start,int& end)
    {
        DWORD wstart=0;
        DWORD wend=0;
        SendMessage(hWnd,EM_GETSEL,(WPARAM)&wstart,(LPARAM)&wend);
        start=wstart;
        end=wend;
    }
    TCHAR* getSelect()
    {
        int start,end;
        getSelect(start,end);

        int n=innerLength();
        TCHAR* buffer=new TCHAR[n];
        TCHAR* ret=new TCHAR[end-start+1];
        GetWindowText(hWnd,buffer,n);

        for(int i=start,j=0;i<end;i++,j++)
        {
            ret[j]=buffer[i];
        }
        ret[end-start]=L'\0';
        return ret;
    }

    int getCursorPosition()
    {
        return 0;
    }
    int setCursorPosition()
    {
        return 0;
    }
    void copy(int start=0,int end=0) // Если start=0 and end=0 в буффер скопируется выделенный фрагмент если start=0 end=-1 скопируется все
    {
        if(start<0) start=0;
        if(end>innerLength()) end=-1;
        PostMessage(hWnd,WM_COPY,start,end);
    }
    void paste(int start=0,int end=0)
    {
        if(start<0) start=0;
        if(end>innerLength()) end=-1;
        PostMessage(hWnd,WM_PASTE,start,end);
    }
    void cut(int start=0,int end=0) // Если start=0 and end=0 в буффер скопируется выделенный фрагмент если start=0 end=-1 скопируется все
    {
        if(start<0) start=0;
        if(end>innerLength()) end=-1;
        PostMessage(hWnd,WM_CUT,start,end);        
    }
    void undo()
    {
        if(this->sendMessage(EM_CANUNDO))
        {
            this->postMessage(EM_UNDO);
        }
    }
};
class Button: public Window
{
public:
    Button(int x,int y,int width,int height,HWND parent,HINSTANCE hInst,DWORD defaultStyle=0)
    {
        this->x=x;
        this->y=y;
        if(width<10) width=10;
        if(height<10) height=10;
        this->width=width;
        this->height=height;
        styles=WS_CHILD | WS_VISIBLE | defaultStyle; 
        hWnd=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,styles,x,y,width,height,parent,HMENU(windowCounter),hInst,0);   
        windowCounter++;
    }
    Button(HWND hWnd)
    {
        this->hWnd=hWnd;
        this->updateFromWindow();
    }
    void setWindowText(char* text)
    {
        TCHAR* t=new TCHAR[strlen(text)];
        wsprintf(t,L"%S",text);
        SetWindowText(hWnd,t);
    }
    void setWindowText(TCHAR* text)
    {
        SetWindowText(hWnd,text);
    }


};
class ListBox: public Window
{
public:
    ListBox(int x,int y,int width,int height,HWND parent,HINSTANCE hInst,DWORD defaultStyle=0)
    {
        this->x=x;
        this->y=y;
        if(width<10) width=10;
        if(height<10) height=10;
        this->width=width;
        this->height=height;
        this->styles=WS_CHILD | WS_VISIBLE | defaultStyle;
        this->hWnd=CreateWindowEx(WS_EX_CLIENTEDGE,L"LISTBOX",0,styles,x,y,width,height,parent,NULL,hInst,0);
    }
    ListBox(HWND hWnd)
    {
        this->hWnd=hWnd;
        this->updateFromWindow();
    }
    void push(const char* str)
    {
        TCHAR* buf=new TCHAR[strlen(str)+1];
        wsprintf(buf,L"%S",str);
        SendMessage(hWnd,LB_ADDSTRING,0,LPARAM(buf));
    }
    void push(const TCHAR* str)
    {

        /*TCHAR* buf=new TCHAR[lstrlen(str)+1];
        wsprintf(buf,L"%S",str);
        buf[lstrlen(str)]='\0';*/
        this->sendMessage(LB_ADDSTRING,0,LPARAM(str)); 
    }
    void push(const int n)
    {
        TCHAR* buf=new TCHAR[20];
        wsprintf(buf,L"%d",n);
        this->sendMessage(LB_ADDSTRING,0,(LPARAM)buf);
    }
    int find(const char* str,int start=0)
    {
        TCHAR* buf=new TCHAR[strlen(str)];
        wsprintf(buf,L"%S",str);
        LRESULT index=this->sendMessage(LB_FINDSTRING,WPARAM(start),LPARAM(buf));
        if(index==LB_ERR)
            return -1;
        else
            return int(index);
    }
    int getSelected()
    {
        return (int) this->sendMessage(LB_GETCURSEL,0,0);
    }
    int numOfSelected()
    {
        return (int) this->sendMessage(LB_GETSELCOUNT,0,0);
    }
    int* getSelectItems()
    {
        int n=this->numOfSelected();
        int* mas=new int[n];
        this->sendMessage(LB_GETSELITEMS,(WPARAM)n,(LPARAM)mas);
        return mas;
    }

};
class ProgressBar: public Window
{
private:
    int progress;
    int min;
    int max;
    int step;
public:
    ProgressBar(int x,int y,int width,int height,HWND parent,HINSTANCE hInst,DWORD defaultStyle=0)
    {
        this->x=x;
        this->y=y;
        this->progress=0;
        this->min=0;
        this->max=100;
        this->step=10;
        if(width<10) width=10;
        if(height<10) height=10;
        this->width=width;
        this->height=height;
        this->styles=WS_CHILD | WS_VISIBLE | WS_BORDER | defaultStyle;         
        hWnd=CreateWindowEx(0,PROGRESS_CLASS,NULL,styles,this->x,this->y,this->width,this->height,parent,NULL,hInst,NULL);
    }
    ProgressBar(HWND hWnd)
    {
        this->hWnd=hWnd;
        this->updateFromWindow();
    }
    void setStep(int n)
    {
        if(n>max) n=max;
        this->step=n;
        SendMessage(hWnd,PBM_SETSTEP,WPARAM(n),0);
    }
    void setMinMax(int min=0,int max=100)
    {
        if(min<0) min=0;
        if(max>min) max=min+1;
        this->max=max;
        this->min=min;
        this->sendMessage(PBM_SETRANGE,0, MAKELPARAM(this->min,this->max));
    }
    void setMin(int min)
    {
        if(min<0) min=0;
        if(min<this->max)
        {
            this->setMinMax(min,this->max);
        }
    }
    void setMax(int max)
    {
        if(max>this->min)
        {
            this->setMinMax(this->min,max);
        }
    }
    /////// геттеры
    int getPos()
    {
        return progress;
    }
    int getMin()
    {
        return min;
    }
    int getMax()
    {
        return max;
    }
    int getStep()
    {
        return step;
    }
    ////// сеттеры
    void setColor(COLORREF color)
    {
        SendMessage(hWnd,PBM_SETBARCOLOR,0, LPARAM(color));
        this->update();

    }
    void setBgColor(COLORREF color)
    {
        SendMessage(hWnd,PBM_SETBKCOLOR,0, LPARAM(color));
        this->update();
    }
    ////// Влияет на прогресс
    void setPos(int pos)  // pos=-1 сбрасывает состояние pos=-2 заполняет полностью
    {
        if(pos==-1)
        {
            this->setPos(this->min);
            return;
        }
        else if(pos==-2)
        {
            this->setPos(this->max);
            return;
        }
        if(pos>=this->min && pos<=this->max)
        {
            progress=pos;
            this->sendMessage(PBM_SETPOS,pos);
        }
    }
    void nextStep()
    {
        progress+=step;
        if(progress>max || progress<min)
        {

            if(progress>max) progress=max;
            if(progress<min) progress=min;
            this->setPos(progress);
        }
        else
            this->sendMessage(PBM_STEPIT);      
    }
    void add(int n)
    {
        progress+=n;
        if(progress>max || progress<min)
        {
            if(progress>max) progress=max;
            if(progress<min) progress=min;
            this->setPos(progress);
        }
        else
            this->sendMessage(PBM_DELTAPOS,n);
    }
    void operator+=(int n)
    {
        this->add(n);
    }

};
class SliderControl: public Window
{
private:
    int progress;
    int min;
    int max;
    int step;
public:
    SliderControl(int x,int y,int width,int height,HWND parent,HINSTANCE hInst,DWORD defaultStyle=0)
    {
        this->x=x;
        this->y=y;
        this->progress=0;
        this->min=0;
        this->max=100;
        this->step=10;
        if(width<10) width=10;
        if(height<10) height=10;
        this->width=width;
        this->height=height;
        this->styles=WS_CHILD | WS_VISIBLE | WS_BORDER | defaultStyle;         
        hWnd=CreateWindowEx(0,TRACKBAR_CLASS,NULL,styles,this->x,this->y,this->width,this->height,parent,NULL,hInst,NULL);
    }
    void setStep(int n)
    {
        if(n>max) n=max;
        this->step=n;
        SendMessage(hWnd,TBM_SETTICFREQ,WPARAM(this->step),0);
    }
    void setMinMax(int min=0,int max=100)
    {
        if(min<0) min=0;
        if(max>min) max=min+1;
        this->max=max;
        this->min=min;
        this->sendMessage(TBM_SETRANGE,0, MAKELPARAM(this->min,this->max));
    }
    void setMin(int min)
    {
        if(min<0) min=0;
        if(min<this->max)
        {
            this->setMinMax(min,this->max);
        }
    }
    void setMax(int max)
    {
        if(max>this->min)
        {
            this->setMinMax(this->min,max);
        }
    }
    /////// геттеры
    int getPos()
    {
        return progress;
    }
    int getMin()
    {
        return min;
    }
    int getMax()
    {
        return max;
    }
    int getStep()
    {
        return step;
    }
    ////// Влияет на прогресс
    void setPos(int pos)  // pos=-1 сбрасывает состояние pos=-2 заполняет полностью
    {
        if(pos==-1)
        {
            this->setPos(this->min);
            return;
        }
        else if(pos==-2)
        {
            this->setPos(this->max);
            return;
        }
        if(pos>=this->min && pos<=this->max)
        {
            progress=pos;
            this->sendMessage(TBM_SETPOS,0,pos);
        }
    }
    void nextStep()
    {
        progress+=step;
        if(progress>max || progress<min)
        {

            if(progress>max) progress=max;
            if(progress<min) progress=min;
            this->setPos(progress);
        }
        else
        {
            this->setPos(progress);
        }    
    }
    void add(int n)
    {
        progress+=n;
        if(progress>max || progress<min)
        {
            if(progress>max) progress=max;
            if(progress<min) progress=min;
            this->setPos(progress);
        }
        else
            this->setPos(progress);
    }
    void operator+=(int n)
    {
        this->add(n);
    }

};
class SpinControl: public Window
{
private:
    int progress;
    int min;
    int max;
    int step;
    int spinWidth,spinHeight;
    HWND hBuddy;
    /*TextField* buddy;*/
public:
    SpinControl(int x,int y,int width,int height,HWND parent,HINSTANCE hInst,DWORD defaultStyle=0,int min=0,int max=100,int spinWidth=20,int spinHeight=-1)
    {


        this->x=x;
        this->y=y;
        this->progress=0;
        this->min=0;
        this->max=100;
        this->step=10;
        if(width<20) width=20;
        if(height<20) height=20;
        this->styles=WS_CHILD | WS_VISIBLE | WS_BORDER | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | defaultStyle;    
        if(min<max)
        {
            this->min=min;
            this->max=max;
        }
        else
        {
            this->min=min;
            this->max=this->min+1;
        }

        //////////

        if(spinWidth>this->width)
        {
            this->spinWidth=this->width-10;
        }
        else
        {
            this->spinWidth=spinWidth;
        }
        //////////
        this->width=width;
        this->height=height;
        // this->buddy=new TextField(this->x,this->y,this->width,this->height,parent,0,0);
        this->hBuddy=CreateWindowEx(WS_EX_CLIENTEDGE,L"EDIT",NULL,WS_VISIBLE | WS_CHILD ,this->x,this->y,this->width,this->height,parent,0,NULL,NULL);
        this->hWnd=CreateUpDownControl(this->styles,0,0,this->height,this->spinWidth,parent,0,NULL,hBuddy,this->max,this->min,this->progress);
        //this->hWnd=new TextField(this->x,this->y,this->width,this->height,parent,hInst,FieldStyle.NumberOnly);
        //this->hWnd=CreateUpDownControl(this->styles,0,0,this->spinWidth,this->spinHeight,parent,0,hInst,hBuddy->getWindow(),this->max,this->min,this->progress);
    }
    void setValue(int value)
    {
        if(value>=this->min && value<=this->max)
        {
            this->progress=value;
            //this->sendMessage(UDM_SETPOS,0,WPARAM(this->progress));
            SendMessage(hWnd,UDM_SETPOS,0,LPARAM(this->progress));
        }
    }
    int getValue()
    {
        bool t=false;
        return (int)SendMessage(hWnd,UDM_GETPOS,0,0);
    }
    void hide()
    {
        this->progress=getValue();
        DestroyWindow(hWnd);
        DestroyWindow(hBuddy);
    }
    void show(HWND parent)
    {  
        this->hBuddy=CreateWindowEx(WS_EX_CLIENTEDGE,L"EDIT",NULL,WS_VISIBLE | WS_CHILD ,this->x,this->y,this->width,this->height,parent,0,NULL,NULL);
        this->hWnd=CreateUpDownControl(this->styles,0,0,this->height,this->spinWidth,parent,0,NULL,hBuddy,this->max,this->min,this->progress);        
    }
    void setStep()
    {

    }
    int getStep()
    {
        return this->step;
    }
};
#define XSTART 70
#define YSTART 10
#define WIDTH 100
#define STATICWIDTH 55
#define HEIGHT 20
#define TOP 5
#define SHOWBUT 5000
#define DRAWBUT 5001
#define SETKA 5002
HINSTANCE hInst;
SpinControl* width,*height,*size;

HWND heightStatic,widthStatic,sizeStatic;
HWND showBut=NULL;
HWND pPentomeno,iPentomeno,lPentomeno;
HWND currentPento=NULL;
HWND setkaCheck;
bool drawSetka=false;
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HDC dc;
TCHAR szClassWindow[] = TEXT("(c) Podkolzzzin");	/* Имя класса окна */
#define MENUWIDTH 215
#define MENUHEIGHT 625
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
    HWND hWnd;
    MSG lpMsg;
    WNDCLASSEX wcl;

    // 1. Определение класса окна
    wcl.cbSize = sizeof(wcl);	// размер структуры WNDCLASSEX
    // Перерисовать всё окно, если изменён размер по горизонтали или по вертикали
    wcl.style = CS_HREDRAW | CS_VREDRAW;	// CS (Class Style) - стиль класса окна
    wcl.lpfnWndProc = WindowProc;	// адрес оконной процедуры
    wcl.cbClsExtra = 0;		// используется Windows 
    wcl.cbWndExtra  = 0; 	// используется Windows 
    wcl.hInstance = hInst;	// дескриптор данного приложения
    wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// загрузка стандартной иконки
    wcl.hCursor = LoadCursor(NULL, IDC_ARROW);	// загрузка стандартного курсора	
    wcl.hbrBackground = (HBRUSH) GetStockObject(RGB(0,0,0));	// заполнение окна белым цветом
    wcl.lpszMenuName = NULL;	// приложение не содержит меню
    wcl.lpszClassName = szClassWindow;	// имя класса окна
    wcl.hIconSm = NULL;	// отсутствие маленькой иконки для связи с классом окна

    // 2. Регистрация класса окна
    if (!RegisterClassEx(&wcl))
        return 0; // при неудачной регистрации - выход

    // 3. Создание окна
    // создается окно и  переменной hWnd присваивается дескриптор окна
    hWnd = CreateWindowEx(
        0,		// расширенный стиль окна
        szClassWindow,	//имя класса окна
        TEXT("(c) Podkolzzzin"), // заголовок окна
        WS_OVERLAPPEDWINDOW,				// стиль окна
        /* Заголовок, рамка, позволяющая менять размеры, системное меню, кнопки развёртывания и свёртывания окна  */
        CW_USEDEFAULT,	// х-координата левого верхнего угла окна
        CW_USEDEFAULT,	// y-координата левого верхнего угла окна
        MENUWIDTH,	// ширина окна
        MENUHEIGHT,	// высота окна
        NULL,			// дескриптор родительского окна
        NULL,			// дескриптор меню окна
        hInst,		// идентификатор приложения, создавшего окно
        NULL);		// указатель на область данных приложения

    // 4. Отображение окна
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd); // перерисовка окна

    // 5. Запуск цикла обработки сообщений
    SendMessage(hWnd,WM_INITDIALOG,0,0);
    dc=GetDC(hWnd);
    while(GetMessage(&lpMsg, NULL, 0, 0)) // получение очередного сообщения из очереди сообщений
    {
        TranslateMessage(&lpMsg);	// трансляция сообщения
        DispatchMessage(&lpMsg);	// диспетчеризация сообщений
    }
    return lpMsg.wParam;
}	
int dir[4][2]={{1,0},{0,1}, {-1,0}, {0,-1}};
void line(int x1,int y1,int x2, int y2)
{
    MoveToEx(dc,x1,y1,0);
    LineTo(dc,x2,y2); 
}
struct pPentomenon
{
    int type;
    int x,y;
};
struct pPentomenonSave
{
    std::vector<pPentomenon> save;
    int width;
    int height;
};
std::vector<pPentomenonSave> PPentomenonLibrary;
std::vector<pPentomenonSave> LPentomenonLibrary;
HBRUSH colors[12];
int colorIndex=0;
//===========================draw functions
void drawPentomino(int x,int y,int size,int type)
{
    colorIndex%=12;

    int direction=-1;
    int currentX=x,currentY=y;
    int prevX=x,prevY=y;
    HPEN hPen; //Объявляется кисть
    hPen=CreatePen(PS_SOLID, 1, RGB(0,0,0)); //Создаётся объект
    SelectObject(dc, hPen); //Объект делается текущим
    RECT fillRect;
    if(type==1)
    {
        fillRect.top=y+size;
        fillRect.left=x;
        fillRect.bottom=y+3*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y;
        fillRect.left=x;
        fillRect.bottom=y+size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x,y,0);
        LineTo(dc,x+size,y);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x+2*size,y+size);
        LineTo(dc,x+2*size,y+3*size);
        LineTo(dc,x,y+3*size);
        LineTo(dc,x,y);
    }
    else if(type==2)
    {
        fillRect.top=y+size;
        fillRect.left=x;
        fillRect.bottom=y+3*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y;
        fillRect.left=x+size;
        fillRect.bottom=y+size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);

        MoveToEx(dc,x,y+size,0);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x+size,y);
        LineTo(dc,x+2*size,y);
        LineTo(dc,x+2*size,y+3*size);
        LineTo(dc,x,y+3*size);
        LineTo(dc,x,y+size);        
    }
    else if(type==3)
    {
        fillRect.top=y;
        fillRect.left=x;
        fillRect.bottom=y+2*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y;
        fillRect.left=x+2*size;
        fillRect.bottom=y+size;
        fillRect.right=x+3*size;
        FillRect(dc,&fillRect,colors[colorIndex]);    


        MoveToEx(dc,x,y,0);
        LineTo(dc,x+3*size,y);
        LineTo(dc,x+3*size,y+size);
        LineTo(dc,x+2*size,y+size);
        LineTo(dc,x+2*size,y+2*size);
        LineTo(dc,x,y+2*size);
        LineTo(dc,x,y);
    }
    else if(type==4)
    {
        fillRect.top=y;
        fillRect.left=x;
        fillRect.bottom=y+2*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y+size;
        fillRect.left=x+2*size;
        fillRect.bottom=y+2*size;
        fillRect.right=x+3*size;
        FillRect(dc,&fillRect,colors[colorIndex]);    


        MoveToEx(dc,x,y,0);
        LineTo(dc,x+2*size,y);
        LineTo(dc,x+2*size,y+size);
        LineTo(dc,x+3*size,y+size);
        LineTo(dc,x+3*size,y+2*size);
        LineTo(dc,x,y+2*size);
        LineTo(dc,x,y);        
    }
    else if(type==5)
    {
        fillRect.top=y;
        fillRect.left=x;
        fillRect.bottom=y+2*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y+2*size;
        fillRect.left=x+size;
        fillRect.bottom=y+3*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);



        MoveToEx(dc,x,y,0);
        LineTo(dc,x+2*size,y);
        LineTo(dc,x+2*size,y+3*size);
        LineTo(dc,x+size,y+3*size);
        LineTo(dc,x+size,y+2*size);
        LineTo(dc,x,y+2*size);
        LineTo(dc,x,y);          
    }
    else if(type==6)
    {
        fillRect.top=y;
        fillRect.left=x;
        fillRect.bottom=y+2*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y+2*size;
        fillRect.left=x+size;
        fillRect.bottom=y+3*size;
        fillRect.right=x;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x,y,0);
        LineTo(dc,x+2*size,y);
        LineTo(dc,x+2*size,y+2*size);
        LineTo(dc,x+size,y+2*size);
        LineTo(dc,x+size,y+3*size);
        LineTo(dc,x,y+3*size);
        LineTo(dc,x,y);         
    }
    else if(type==7)
    {
        fillRect.top=y;
        fillRect.left=x+size;
        fillRect.bottom=y+2*size;
        fillRect.right=x+3*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y+size;
        fillRect.left=x;
        fillRect.bottom=y+2*size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x+size,y,0);
        LineTo(dc,x+3*size,y);
        LineTo(dc,x+3*size,y+2*size);
        LineTo(dc,x,y+2*size);
        LineTo(dc,x,y+size);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x+size,y);

    }
    else if(type==8)
    {
        fillRect.top=y;
        fillRect.left=x+size;
        fillRect.bottom=y+2*size;
        fillRect.right=x+3*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        fillRect.top=y;
        fillRect.left=x;
        fillRect.bottom=y+size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x,y,0);
        LineTo(dc,x+3*size,y);
        LineTo(dc,x+3*size,y+2*size);
        LineTo(dc,x+size,y+2*size);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x,y+size);
        LineTo(dc,x,y);         
    }
    colorIndex++;
}
void drawIPentomino(int x,int y,int size,int type)
{
    colorIndex%=12;
    HPEN hPen; //Объявляется кисть
    hPen=CreatePen(PS_SOLID, 1, RGB(0,0,0)); //Создаётся объект
    SelectObject(dc, hPen); //Объект делается текущим
    SelectObject(dc, colors[colorIndex]);
    RECT fillRect;
    if(type==1)
    {
        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+size;
        fillRect.right=x+5*size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        MoveToEx(dc,x,y,0);
        LineTo(dc,x+5*size,y);
        LineTo(dc,x+5*size,y+size);
        LineTo(dc,x,y+size);
        LineTo(dc,x,y);
    }
    else if(type==2)
    {

        MoveToEx(dc,x,y,0);
        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+5*size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);
        MoveToEx(dc,x,y,0);
        LineTo(dc,x,y+5*size);
        LineTo(dc,x+size,y+5*size);
        LineTo(dc,x+size,y);
        LineTo(dc,x,y);
    }
    colorIndex++;
}
void drawLPentamino(int x,int y,int size,int type)
{
    colorIndex%=12;
    HPEN hPen; //Объявляется кисть
    hPen=CreatePen(PS_SOLID, 1, RGB(0,0,0)); //Создаётся объект
    SelectObject(dc, hPen); //Объект делается текущим
    SelectObject(dc, colors[colorIndex]);
    RECT fillRect;
    if(type==1)
    {
        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);

        fillRect.left=x;
        fillRect.top=y+size;
        fillRect.bottom=y+2*size;
        fillRect.right=x+4*size;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x,y,0);
        LineTo(dc,x,y+2*size);
        LineTo(dc,x+4*size,y+2*size);
        LineTo(dc,x+4*size,y+size);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x+size,y);
        LineTo(dc,x,y);
    }
    else if(type==2)
    {

        fillRect.left=x+3*size;
        fillRect.top=y;
        fillRect.bottom=y+size;
        fillRect.right=x+4*size;
        FillRect(dc,&fillRect,colors[colorIndex]);

        fillRect.left=x;
        fillRect.top=y+size;
        fillRect.bottom=y+2*size;
        fillRect.right=x+4*size;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x,y+size,0);
        LineTo(dc,x,y+2*size);
        LineTo(dc,x+4*size,y+2*size);
        LineTo(dc,x+4*size,y);
        LineTo(dc,x+3*size,y);
        LineTo(dc,x+3*size,y+size);
        LineTo(dc,x,y+size);
    }
    else if(type==3)
    {
        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+size;
        fillRect.right=x+4*size;
        FillRect(dc,&fillRect,colors[colorIndex]);

        fillRect.left=x+3*size;
        fillRect.top=y+size;
        fillRect.bottom=y+2*size;
        fillRect.right=x+4*size;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x,y,0);
        LineTo(dc,x+4*size,y);
        LineTo(dc,x+4*size,y+2*size);
        LineTo(dc,x+3*size,y+2*size);
        LineTo(dc,x+3*size,y+size);
        LineTo(dc,x,y+size);
        LineTo(dc,x,y);        
    }
    else if(type==4)
    {
        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+size;
        fillRect.right=x+4*size;
        FillRect(dc,&fillRect,colors[colorIndex]);

        fillRect.left=x;
        fillRect.top=y+size;
        fillRect.bottom=y+2*size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);


        MoveToEx(dc,x,y,0);
        LineTo(dc,x,y+2*size);
        LineTo(dc,x+size,y+2*size);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x+4*size,y+size);
        LineTo(dc,x+4*size,y);
        LineTo(dc,x,y);     
    }
    else if(type==5)
    {
        fillRect.left=x+size;
        fillRect.top=y;
        fillRect.bottom=y+4*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);

        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]); 
 
        MoveToEx(dc,x,y,0);
        LineTo(dc,x+2*size,y);
        LineTo(dc,x+2*size,y+4*size);
        LineTo(dc,x+size,y+4*size);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x,y+size);
        LineTo(dc,x,y);

    }
    else if(type==6)
    {
        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+4*size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);  

        fillRect.left=x+size;
        fillRect.top=y;
        fillRect.bottom=y+size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);  

        MoveToEx(dc,x,y,0);
        LineTo(dc,x+2*size,y);
        LineTo(dc,x+2*size,y+size);
        LineTo(dc,x+size,y+size);
        LineTo(dc,x+size,y+4*size);
        LineTo(dc,x,y+4*size);
        LineTo(dc,x,y);
    }
    else if(type==7)
    {
        fillRect.left=x;
        fillRect.top=y;
        fillRect.bottom=y+4*size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);  

        fillRect.left=x+size;
        fillRect.top=y+3*size;
        fillRect.bottom=y+4*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]); 

        MoveToEx(dc,x,y,0);
        LineTo(dc,x+size,y);
        LineTo(dc,x+size,y+3*size);
        LineTo(dc,x+2*size,y+3*size);
        LineTo(dc,x+2*size,y+4*size);
        LineTo(dc,x,y+4*size);
        LineTo(dc,x,y);
    }
    else if(type==8)
    {
        fillRect.left=x+size;
        fillRect.top=y;
        fillRect.bottom=y+4*size;
        fillRect.right=x+2*size;
        FillRect(dc,&fillRect,colors[colorIndex]);

        fillRect.left=x;
        fillRect.top=y+3*size;
        fillRect.bottom=y+4*size;
        fillRect.right=x+size;
        FillRect(dc,&fillRect,colors[colorIndex]);    

        
        MoveToEx(dc,x+size,y,0);
        LineTo(dc,x+2*size,y);
        LineTo(dc,x+2*size,y+4*size);
        LineTo(dc,x,y+4*size);
        LineTo(dc,x,y+3*size);
        LineTo(dc,x+size,y+3*size);
        LineTo(dc,x+size,y);
    }

    colorIndex++;
}
//===========================draw save functions
void drawSave(pPentomenonSave &save,int x,int y, int size)
{
    for(int i=0;i<save.save.size();i++)
    {
        drawPentomino(x+save.save[i].x*size,y+save.save[i].y*size,size,save.save[i].type);
        //Sleep(100);
    }
}
void drawLSave(pPentomenonSave &save,int x,int y, int size)
{
    for(int i=0;i<save.save.size();i++)
    {
        drawLPentamino(x+save.save[i].x*size,y+save.save[i].y*size,size,save.save[i].type);
        //Sleep(100);
    }
}
//==========================setka
void setka(int x,int y,int width,int height,int size,HWND parent)
{
    HPEN hPen; //Объявляется кисть
    hPen=CreatePen(PS_SOLID, 1, RGB(128,128,128)); //Создаётся объект
    SelectObject(dc, hPen); //Объект делается текущим
    SetTextAlign(dc, TA_CENTER);
    SetBkMode(dc, 0);
    TCHAR* num;
    for(int i=0;i<width;i++)
    {
        MoveToEx(dc,x+size*i,y,0);
        LineTo(dc,x+size*i,y+height*size);
        if(i%2==0)
        {
            num=intToTchar(i);
            TextOut(dc,x+size*i,y+height*size,num,lstrlen(num));
        }
    }
    SetTextAlign(dc, TA_RIGHT | TA_BASELINE);
    for(int i=0;i<height;i++)
    {
        MoveToEx(dc,x,y+size*i,0);
        LineTo(dc,x+size*width,y+size*i);  
        if(i%2==0)
        {
            num=intToTchar(i);
            TextOut(dc,x,y+size*i+5,num,lstrlen(num));
        }
    }
}
void GenerateObjectsPpento(int width,int height,int size,HWND hWnd,int marginX=0,int marginY=0)
{

    RECT winRect;
    if(marginX!=0 && marginY!=0)
    {
        GetWindowRect(hWnd,&winRect);
        MoveWindow(hWnd,winRect.left,winRect.top,winRect.right-winRect.left+1,winRect.bottom-winRect.top+1,true);
        UpdateWindow(hWnd);
    }
    TCHAR* temp=new TCHAR[50];
    //wsprintf(temp,L"%d %d",width,height);
    //SetWindowText(hWnd,temp);

    if(marginX==0 && marginY==0)
    {
        if((height%10==0 && width%2!=0 && width>5) || (height%5==0 && width%2!=0 && height>10))
        {
            GetWindowRect(hWnd,&winRect);
            int t=width;
            width=height;
            height=t;
            if(size*height+75>MENUHEIGHT)
            {
                MoveWindow(hWnd,winRect.left,winRect.top,MENUWIDTH+75+size*width,size*height+75,true);
                UpdateWindow(hWnd);
            }
            else
            {
                MoveWindow(hWnd,winRect.left,winRect.top,MENUWIDTH+75+size*width,MENUHEIGHT+20,true);
                UpdateWindow(hWnd);       
            }
        }
    }
    int saveWidth=width;
    int saveHeight=height;
    if(width%5!=0 && height%5!=0)
    {
        MessageBox(hWnd,L"Невозможно замостить данный прямоугольник P-pentamino",L"Pentomino error",MB_OK | MB_ICONERROR);
        return;
    }

    if(width%2==0 && height%5==0)
    {
        for(int j=0;j<height/5;j++)
        {
            for(int i=0;i<width/2;i++)
            {
                drawSave(PPentomenonLibrary[0],MENUWIDTH+5+2*size*i+marginX,5+j*size*5+marginY,size);
            }
        }
    }
    else if(height%2==0 && width%5==0)
    {
        for(int j=0;j<height/2;j++)
        {
            for(int i=0;i<width/5;i++)
            {
                drawSave(PPentomenonLibrary[1],MENUWIDTH+5+5*size*i+marginX,5+j*size*2+marginY,size);
            }
        }            
    }
    /*else  if(width%10==0 && height%2!=0 && height>5)
    {
        for(int j=0;j<height/PPentomenonLibrary[2].height;j++)
        {
            for(int i=0;i<width/PPentomenonLibrary[2].width;i++)
                drawSave(PPentomenonLibrary[2],MENUWIDTH+5+PPentomenonLibrary[2].width*size*i,5+PPentomenonLibrary[2].height*size*j,size);
        }
        
        height-=PPentomenonLibrary[2].height;

        for(int j=0;j<height/2;j++)
        {
            for(int i=0;i<width/5;i++)
            {
                drawSave(PPentomenonLibrary[1],MENUWIDTH+5+5*size*i,5+j*size*2+5*size,size);
            }
        }

    }
    else if(height%10==0 && width%2!=0 && width>5)
    {
        MessageBox(0,L"error",0,0);
        for(int j=0;j<height/PPentomenonLibrary[3].height;j++)
        {
            for(int i=0;i<width/PPentomenonLibrary[3].width;i++)
                drawSave(PPentomenonLibrary[3],MENUWIDTH+5+PPentomenonLibrary[3].width*size*i,5+PPentomenonLibrary[3].height*size*j,size);
        }
        
        
        int tx;
        int ty;
        for(int j=0;j<(width%5)/2;j++)
        {
            for(int i=0;i<height/5;i++)
            {
                tx=MENUWIDTH+5+PPentomenonLibrary[0].width*size*j+(width/PPentomenonLibrary[3].width)*size*PPentomenonLibrary[3].width;
                ty=5+i*size*5;
                drawSave(PPentomenonLibrary[0],tx,ty,size);
            }
        }
    }*/
    else if(width%10==0 && height%2!=0 && height>5)
    {
        for(int i=0;i<width/PPentomenonLibrary[2].width;i++)
        {
            drawSave(PPentomenonLibrary[2],MENUWIDTH+5+i*size*PPentomenonLibrary[2].width+marginX,5+marginY,size);
        }
        height-=PPentomenonLibrary[2].height;
        for(int j=0;j<height/PPentomenonLibrary[1].height;j++)
        {
            for(int i=0;i<width/PPentomenonLibrary[1].width;i++)
            {
                drawSave(PPentomenonLibrary[1],PPentomenonLibrary[2].height*size*i+MENUWIDTH+5+marginX,size*PPentomenonLibrary[2].height+5+size*PPentomenonLibrary[1].height*j+marginY,size);
            }
        }
    }
    else if(width%15==0 && height>=7)
    {
        for(int i=0;i<width/15;i++)
        {
            drawSave(PPentomenonLibrary[4],MENUWIDTH+5+15*size*i+marginX,5+marginY,size);
        }

        height-=7;
        for(int i=0;i<height/2;i++)
        {
            for(int j=0;j<width/5;j++)
            {
                drawSave(PPentomenonLibrary[1],MENUWIDTH+5+size*5*j+marginX,5+i*size*2+7*size+marginY,size);
            }
        }
    }
    else if(width%5==0 && height%2!=0 && width>10)
    {

        drawSave(PPentomenonLibrary[4],MENUWIDTH+5,5,size);
        //Sleep(1000);
        GenerateObjectsPpento(PPentomenonLibrary[4].width,height-PPentomenonLibrary[4].height,size,hWnd,0,PPentomenonLibrary[4].height*size);
        //Sleep(1000);
        GenerateObjectsPpento(width-PPentomenonLibrary[4].width,height,size,hWnd,PPentomenonLibrary[4].width*size,0);
    }
    else
    {
        MessageBox(hWnd,L"Невозможно замостить данный прямоугольник P-pentamino",L"Pentomino error",MB_OK | MB_ICONERROR);
        return;        
    }
    if(drawSetka)
    {
        if(marginX==0 && marginY==0)
            setka(MENUWIDTH+5,5,saveWidth,saveHeight,size,hWnd);
    }
}
void GenerateObjectsIpento(int width,int height,int size,HWND hWnd)
{
    int saveWidth=width;
    int saveHeight=height;
    RECT winRect;
    GetWindowRect(hWnd,&winRect);
    MoveWindow(hWnd,winRect.left,winRect.top,winRect.right-winRect.left+1,winRect.bottom-winRect.top+1,true);
    UpdateWindow(hWnd);
    TCHAR* temp=new TCHAR[50];
    wsprintf(temp,L"%d %d",width,height);
    if(width%5!=0 && height%5!=0)
    {
        MessageBox(hWnd,L"Невозможно замостить данный прямоугольник I-pentamino",L"Pentomino error",MB_OK | MB_ICONERROR);
        return;
    }
    else if(width%5==0)
    {
        for(int i=0;i<width/5;i++)
        {
            for(int j=0;j<height;j++)
            {
                drawIPentomino(MENUWIDTH+5+size*5*i,5+size*j,size,1);
            }
        }
        for(int i=0;i<width%5;i++)
        {
            for(int j=0;j<height/5;j++)
            {
                drawIPentomino(MENUWIDTH+5+size*5*i+width/5*5*size,5+size*j,size,2);
            }
        }
        
    }
    else if(height%5==0)
    {
        for(int i=0;i<width;i++)
        {
            for(int j=0;j<height/5;j++)
            {
                drawIPentomino(MENUWIDTH+5+size*i,5+size*j*5,size,2);
            }
        }
        for(int i=0;i<width/5;i++)
        {
            for(int j=0;j<height%5;j++)
            {
                drawIPentomino(MENUWIDTH+5+size*5*i+width/5*5*size,5+size*j,size,1);
            }
        }        
    }
    if(drawSetka)
    {
        setka(MENUWIDTH+5,5,saveWidth,saveHeight,size,hWnd);
    }
}
void GenerateObjectsLpento(int width,int height,int size,HWND hWnd,int marginX=0,int marginY=0)
{
    RECT winRect;
    if(marginX!=0 && marginY!=0)
    {
        GetWindowRect(hWnd,&winRect);
        MoveWindow(hWnd,winRect.left,winRect.top,winRect.right-winRect.left+1,winRect.bottom-winRect.top+1,true);
        UpdateWindow(hWnd);
    }
    TCHAR* temp=new TCHAR[50];
    if(marginX==0 && marginY==0)
    {
        if((height%10==0 && width%2!=0 && width>5) || (height%5==0 && width%2!=0 && height>10))
        {
            GetWindowRect(hWnd,&winRect);
            int t=width;
            width=height;
            height=t;
            if(size*height+75>MENUHEIGHT)
            {
                MoveWindow(hWnd,winRect.left,winRect.top,MENUWIDTH+75+size*width,size*height+75,true);
                UpdateWindow(hWnd);
            }
            else
            {
                MoveWindow(hWnd,winRect.left,winRect.top,MENUWIDTH+75+size*width,MENUHEIGHT+20,true);
                UpdateWindow(hWnd);       
            }
        }
    }
    int saveWidth=width;
    int saveHeight=height;
    if(width%5!=0 && height%5!=0)
    {
        MessageBox(hWnd,L"Невозможно замостить данный прямоугольник L-pentamino",L"Pentomino error",MB_OK | MB_ICONERROR);
        return;
    }

    if(width%2==0 && height%5==0)
    {
        for(int j=0;j<height/5;j++)
        {
            for(int i=0;i<width/2;i++)
            {
                drawLSave(LPentomenonLibrary[0],MENUWIDTH+5+2*size*i+marginX,5+j*size*5+marginY,size);
            }
        }
    }
    else if(height%2==0 && width%5==0)
    {
        for(int j=0;j<height/2;j++)
        {
            for(int i=0;i<width/5;i++)
            {
                drawLSave(LPentomenonLibrary[1],MENUWIDTH+5+5*size*i+marginX,5+j*size*2+marginY,size);
            }
        }            
    }
    else if(width%10==0 && height%2!=0 && height>5)
    {
        for(int i=0;i<width/LPentomenonLibrary[2].width;i++)
        {
            drawLSave(LPentomenonLibrary[2],MENUWIDTH+5+i*size*LPentomenonLibrary[2].width+marginX,5+marginY,size);
        }
        height-=LPentomenonLibrary[2].height;
        for(int j=0;j<height/LPentomenonLibrary[1].height;j++)
        {
            for(int i=0;i<width/PPentomenonLibrary[1].width;i++)
            {
                drawLSave(LPentomenonLibrary[1],LPentomenonLibrary[2].height*size*i+MENUWIDTH+5+marginX,size*LPentomenonLibrary[2].height+5+size*LPentomenonLibrary[1].height*j+marginY,size);
            }
        }
    }
    else if(width%15==0 && height>=7)
    {
        for(int i=0;i<width/15;i++)
        {
            drawLSave(LPentomenonLibrary[4],MENUWIDTH+5+15*size*i+marginX,5+marginY,size);
        }

        height-=7;
        for(int i=0;i<height/2;i++)
        {
            for(int j=0;j<width/5;j++)
            {
                drawLSave(LPentomenonLibrary[1],MENUWIDTH+5+size*5*j+marginX,5+i*size*2+7*size+marginY,size);
            }
        }
    }
    else if(width%5==0 && height%2!=0 && width>10)
    {

        drawLSave(LPentomenonLibrary[4],MENUWIDTH+5,5,size);
        //Sleep(1000);
        GenerateObjectsLpento(LPentomenonLibrary[4].width,height-LPentomenonLibrary[4].height,size,hWnd,0,LPentomenonLibrary[4].height*size);
        //Sleep(1000);
        GenerateObjectsLpento(width-LPentomenonLibrary[4].width,height,size,hWnd,LPentomenonLibrary[4].width*size,0);
    }
    else
    {
        MessageBox(hWnd,L"Невозможно замостить данный прямоугольник L-pentamino",L"Pentomino error",MB_OK | MB_ICONERROR);
        return;        
    }
    if(drawSetka)
    {
        if(marginX==0 && marginY==0)
            setka(MENUWIDTH+5,5,saveWidth,saveHeight,size,hWnd);
    }
}
void loadPPentomenonLibrary()
{
    std::ifstream file("pPentomenon.txt");
    pPentomenonSave tSave;
    pPentomenon pPento;
    int num;
    while(!file.fail())
    {
        file>>num;
        file>>tSave.width;
        file>>tSave.height;
        for(int i=0;i<num;i++)
        {
            file>>pPento.x;
            file>>pPento.y;
            file>>pPento.type;
            tSave.save.push_back(pPento);
        }
        PPentomenonLibrary.push_back(tSave);
        tSave.save.clear();
    }
}
void loadLPentomenonLibrary()
{
    std::ifstream file("lPentomenon.txt");
    pPentomenonSave tSave;
    pPentomenon pPento;
    int num;
    while(!file.fail())
    {
        file>>num;
        file>>tSave.width;
        file>>tSave.height;
        for(int i=0;i<num;i++)
        {
            file>>pPento.x;
            file>>pPento.y;
            file>>pPento.type;
            tSave.save.push_back(pPento);
        }
        LPentomenonLibrary.push_back(tSave);
        tSave.save.clear();
    }
}
ListBox* logicalDisks;
#define BUTTON_START YSTART+4*(HEIGHT+TOP)
#define BUTTON_HEIGHT 80
#define BUTTON_WIDTH 100
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    int result;
    switch(uMessage)
    {
    case WM_INITDIALOG:
        {
            //==================init colors
            colors[0]=CreateSolidBrush(RGB(0,0,0));
            colors[1]=CreateSolidBrush(RGB(255,255,255));
            colors[2]=CreateSolidBrush(RGB(149,149,149));
            colors[3]=CreateSolidBrush(RGB(90,90,149));
            colors[4]=CreateSolidBrush(RGB(0,0,255));
            colors[5]=CreateSolidBrush(RGB(255,138,0));
            colors[6]=CreateSolidBrush(RGB(255,240,0));
            colors[7]=CreateSolidBrush(RGB(100,0,0));
            colors[8]=CreateSolidBrush(RGB(180,255,18));
            colors[9]=CreateSolidBrush(RGB(150,0,30));
            colors[10]=CreateSolidBrush(RGB(255,0,0));
            colors[11]=CreateSolidBrush(RGB(240,0,255));
            //==================init window
            CreateWindowEx(0,L"STATIC",0,WS_CHILD | WS_VISIBLE,0,0,10000,10000,hWnd,0,hInst,0);
            width=new SpinControl(XSTART,YSTART,WIDTH,HEIGHT,hWnd,hInst,0,0,5000);
            height=new SpinControl(XSTART,YSTART+HEIGHT+TOP,WIDTH,HEIGHT,hWnd,hInst,0,0,5000);


            height->setValue(0);
            size=new SpinControl(XSTART,YSTART+2*(HEIGHT+TOP),WIDTH,HEIGHT,hWnd,hInst);
            heightStatic=CreateWindowEx(0,L"STATIC",L"Ширина",WS_CHILD | WS_VISIBLE,XSTART-STATICWIDTH,YSTART,STATICWIDTH,HEIGHT,hWnd,0,hInst,0);
            widthStatic=CreateWindowEx(0,L"STATIC",L"Высота",WS_CHILD | WS_VISIBLE,XSTART-STATICWIDTH,YSTART+(HEIGHT+TOP),STATICWIDTH,HEIGHT,hWnd,0,hInst,0);
            sizeStatic=CreateWindowEx(0,L"STATIC",L"Размер",WS_CHILD | WS_VISIBLE,XSTART-STATICWIDTH,YSTART+2*(HEIGHT+TOP),STATICWIDTH,HEIGHT,hWnd,0,hInst,0);
            sizeStatic=CreateWindowEx(0,L"STATIC",L"Сетка",WS_CHILD | WS_VISIBLE,XSTART-STATICWIDTH,YSTART+3*(HEIGHT+TOP),STATICWIDTH,HEIGHT,hWnd,0,hInst,0);
            setkaCheck=CreateWindowEx(0,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,XSTART,YSTART+3*(HEIGHT+TOP)+3,10,10,hWnd,(HMENU)SETKA,hInst,0);

            pPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,0,BUTTON_START,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)DRAWBUT,hInst,0);
            iPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,BUTTON_WIDTH,BUTTON_START,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,0,hInst,0);

            lPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,0,BUTTON_START+BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,0,hInst,0);
            HWND nPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,BUTTON_WIDTH,BUTTON_START+BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0);            
            HWND fPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,0,BUTTON_START+2*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0); 
            HWND tPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,BUTTON_WIDTH,BUTTON_START+2*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0);
            HWND uPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,0,BUTTON_START+3*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0);
            HWND vPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,BUTTON_WIDTH,BUTTON_START+3*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0);
            HWND wPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,0,BUTTON_START+4*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0);
            HWND xPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,BUTTON_WIDTH,BUTTON_START+4*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0);
            HWND yPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,0,BUTTON_START+5*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5005,hInst,0);
            HWND zPentomeno=CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",0,WS_CHILD | WS_VISIBLE | BS_BITMAP,BUTTON_WIDTH,BUTTON_START+5*BUTTON_HEIGHT,BUTTON_WIDTH,BUTTON_HEIGHT,hWnd,(HMENU)5004,hInst,0);
            
            SendMessage(lPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP2)));           
            SendMessage(iPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP1)));
            SendMessage(pPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP3)));
            SendMessage(xPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP4)));
            SendMessage(yPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP5)));
            SendMessage(tPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP6)));
            SendMessage(zPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP7)));
            SendMessage(fPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP8)));
            SendMessage(uPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP9)));
            SendMessage(vPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP10)));
            SendMessage(wPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP11)));
            SendMessage(nPentomeno,BM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP12)));
            size->setValue(10);
            pPentomenonSave tempSave;
            pPentomenon tempPent;
            loadPPentomenonLibrary();
            loadLPentomenonLibrary();
            //
            break;
        }

    case WM_COMMAND:
        {
            if(LOWORD(wParam)==5004)
            {
                MessageBox(hWnd,L"Невозможно замостить указанный прямоугольник данным видом пентамино",L"Pentamino error",MB_OK | MB_ICONERROR);
                break;
            }
            if(LOWORD(wParam)==5005)
            {
                MessageBox(hWnd,L"Неизвестный результат",L"Pentamino error",MB_OK | MB_ICONERROR);
                break;            
            }

            if((HWND)lParam==pPentomeno)
            {
                currentPento=pPentomeno;
                int tWidth=width->getValue();
                int tHeight=height->getValue();
                if(tWidth<tHeight)
                {
                    int temp=tWidth;
                    tWidth=tHeight;
                    tHeight=temp;
                }
                if(tHeight%15==0 && tWidth>=7)
                {
                    int temp=tWidth;
                    tWidth=tHeight;
                    tHeight=temp;                    
                }
                int newWidth=MENUWIDTH+tWidth*size->getValue()+20;
                int newHeight=tHeight*size->getValue()+40;
                if(newHeight<MENUHEIGHT-20) newHeight=MENUHEIGHT;
                MoveWindow(hWnd,0,0,newWidth+20,newHeight+20,true);
                UpdateWindow(hWnd);
                GenerateObjectsPpento(tWidth,tHeight,size->getValue(),hWnd);
            }
            else if((HWND)lParam==iPentomeno)
            {
                currentPento=iPentomeno;
                int tWidth=width->getValue();
                int tHeight=height->getValue();
                if(tWidth<tHeight)
                {
                    int temp=tWidth;
                    tWidth=tHeight;
                    tHeight=temp;
                }
                int newWidth=MENUWIDTH+tWidth*size->getValue()+20;
                int newHeight=tHeight*size->getValue()+40;
                if(newHeight<MENUHEIGHT-20) newHeight=MENUHEIGHT;
                MoveWindow(hWnd,0,0,newWidth+20,newHeight+20,true);
                UpdateWindow(hWnd);
                GenerateObjectsIpento(tWidth,tHeight,size->getValue(),hWnd);
            }
            else if((HWND)lParam==lPentomeno)
            {
                currentPento=lPentomeno;
                int tWidth=width->getValue();
                int tHeight=height->getValue();
                if(tWidth<tHeight)
                {
                    int temp=tWidth;
                    tWidth=tHeight;
                    tHeight=temp;
                }
                if(tHeight%15==0 && tWidth>=7)
                {
                    int temp=tWidth;
                    tWidth=tHeight;
                    tHeight=temp;                    
                }
                int newWidth=MENUWIDTH+tWidth*size->getValue()+20;
                int newHeight=tHeight*size->getValue()+40;
                if(newHeight<MENUHEIGHT-20) newHeight=MENUHEIGHT;
                MoveWindow(hWnd,0,0,newWidth+20,newHeight+20,true);
                UpdateWindow(hWnd);
                GenerateObjectsLpento(tWidth,tHeight,size->getValue(),hWnd);                
            }
            
            if(LOWORD(wParam)==SETKA)
            {
                drawSetka=!drawSetka;
                SendMessage(hWnd,WM_COMMAND,0,LPARAM(currentPento));
            }
            break;
        }
    case WM_DESTROY: // сообщение о завершении программы
        PostQuitMessage(0); // посылка сообщения WM_QUIT
        break;
    default:
        // все сообщения, которые не обрабатываются в данной оконной функции 
        // направляются обратно Windows на обработку по умолчанию
        return DefWindowProc(hWnd, uMessage, wParam, lParam);
    }
    return 0;
}
