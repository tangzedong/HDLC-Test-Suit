/*******************************************************************************
* Copyright:   2011-2012 by Arsaisy.lin
* File name:   LayoutManager.h
* Description: 布局管理器模块，用于管理MS Windows平台的桌面应用程序界面布局，兼
*              容由Win32API或MFC、ATL等与窗口句柄(HWND)相关的库创建的桌面界面程
*              序。本模块依赖于MS Platform SDK和标准C++STL。
*              本文件为免费资源，可以应用于开源、免费和商业等软件应用，不会收取
*              任何费用，但需要注明程序出处。
* Author:      Arsaisy.lin
* Contact:     arsaisy@126.com
* Version:     1.0.1
* Date:        2011/10/26
* History:     2011/11/10
*                1.修正FILL类型分配空间不正确的问题
*******************************************************************************/

#ifndef __LAYOUTMANAGER_H__
#define __LAYOUTMANAGER_H__

#include <Windows.h>
#include <list>
#include <vector>

namespace Win32Layout
{

	/*******************************************************************************
	* SizeType尺寸类型，布局中的占位方式。
	*   PIXEL:      以像素方式占位
	*   PERCENTAGE: 以比例方式占位
	#   FILL:       以填充方式占位
	*******************************************************************************/
	enum SizeType {
		PIXEL,
		PERCENTAGE,
		FILL
	};

	/*******************************************************************************
	* BoxType盒子类型，布局对象的对齐方式。
	*   HORIZONTAL: 水平分布对象
	*   VERTICAL:   垂直分布对象
	*******************************************************************************/
	enum BoxType {
		HORIZONTAL,
		VERTICAL
	};

	/*******************************************************************************
	* LayoutObject布局对象类
	*   LayoutObject类是所有可以进行布局类的基类，需要进行布局的类都需要继承此类。
	*******************************************************************************/
	class LayoutObject {
	public:
		LayoutObject() :_w(0), _h(0), _stw(PIXEL), _sth(PIXEL) {}
		virtual ~LayoutObject() {}
		virtual void Layout(int x, int y, int w, int h) = 0;
		int GetW() { return _w; }
		int GetH() { return _h; }
		SizeType GetWST() { return _stw; }
		SizeType GetHST() { return _sth; }

		/*
		* SetSize函数，设置布局对象的尺寸和尺寸类型。
		*   w:   宽度
		*   h:   高度
		*   stw: 宽度尺寸类型
		*   sth: 高度尺寸类型
		*   宽度与高度由尺寸类型动态决定其大小。如果尺寸类型为像素PIXEL，宽度或高度
		* 表示实际的像素的大小；如果尺寸类型为比例PERCENTAGE，宽度或高度的大小表示
		* 总宽度或总高度的相应比例值；如果尺寸类型为FILL，宽度或高度则尽可能填充所有
		* 的剩余范围。
		*/
		void SetSize(int w, int h, SizeType stw, SizeType sth) {
			_w = w;
			_h = h;
			_stw = stw;
			_sth = sth;
		}

	protected:
		int _w;
		int _h;
		SizeType _stw;
		SizeType _sth;
	};


	/*******************************************************************************
	* Window窗口类，继承于LayoutObject类，负责调整窗口的位置。
	*   通过构造函数与窗口句柄绑定，也可以创建对象后使用Attach与窗口句柄绑定。类销
	* 毁时不会销毁窗口，不需要解除绑定。可以在使用过程中动态绑定其他窗口，但不建议
	* 这样做。
	*******************************************************************************/
	class Window : public LayoutObject {
	public:
		Window(HWND hWnd = NULL) { _hWnd = hWnd; }
		virtual ~Window() {}

		/*
		* Attach函数，与窗口句柄进行绑定。如果构造函数没有指定窗口句柄，可以在需要
		* 时调用此函数进行绑定。也可以不绑定，布局时不发生任何操作。
		*/
		void Attach(HWND hWnd) { _hWnd = hWnd; }

		/*
		* Handle函数，获取已经绑定的窗口句柄。
		*/
		HWND Handle() { return _hWnd; }

		/*
		* Layout布局函数，把对象放置在坐标(x, y)，大小为w*h的范围内。对窗口类则简单
		* 的调用Win32API函数MoveWindow设置对象位置。
		*/
		void Layout(int x, int y, int w, int h) {
			if (::IsWindow(_hWnd))
				SetWindowPos(_hWnd, NULL, x, y, w, h, 0);
		}

	protected:
		/*
		* 窗口句柄，可以为NULL。
		*/
		HWND _hWnd;
	};

	/*******************************************************************************
	* Box布局盒子类，本模块最重要的类，所有布局任务都由此类完成。
	*   该类一般与WINDOWS消息共同使用。使用方法为1.创建该对象；2.调用AddWindow指派要
	* 进行布局的窗口句柄或调用AddHBox、AddVBox增加嵌套的布局盒子；3.在需要的时候（如
	# WM_SIZE消息）时调用盒子的Layout函数进行布局管理。
	*/
	class Box : public LayoutObject {
	public:
		Box(BoxType boxType = HORIZONTAL) {
			_separatorSize = 3;
			_boxType = boxType;
		}

		virtual ~Box() {
			std::list<LayoutObject*>::iterator it = _sublist.begin();
			for (; it != _sublist.end(); it++)
				delete (*it);
			_sublist.clear();
		}

		/*
		* SetType函数，设置盒子的对齐方式，可以是水平或垂直。
		*/
		void SetType(BoxType boxType) {
			_boxType = boxType;
		}

		/*
		* SetSeparatorSize函数，设置分隔条大小，默认为3个像素。
		*/
		void SetSeparatorSize(int size) {
			_separatorSize = size;
		}

		/*
		* AddHBox函数，增加内嵌的水平盒子。
		*   增加时可以指定盒子的大小与对齐方式。增加完成后返回新增加的盒子，可以使用
		* 返回的盒子继续增加对象。由本函数创建的内嵌盒子不可以调用delete删除，它由系
		* 统自动回收。
		*/
		Box* AddHBox(int w, int h, SizeType stw, SizeType sth) {
			Box* box = new Box(HORIZONTAL);
			box->SetSize(w, h, stw, sth);
			_sublist.push_back(box);
			return box;
		}

		/*
		* AddVBox函数，增加内嵌的垂直盒子。
		*   功能与AddHBox功能类似。
		*/
		Box* AddVBox(int w, int h, SizeType stw, SizeType sth) {
			Box* box = new Box(VERTICAL);
			box->SetSize(w, h, stw, sth);
			_sublist.push_back(box);
			return box;
		}

		/*
		* AddWindow函数，增加需要布局的窗口。
		*   调用该函数增加一个需要进行布局的窗口句柄，同时指定大小与尺寸类型，并返回
		* 相应的窗口布局对象，可以保存方便以后更改大小。本函数创建的对象不可以调用
		* delete删除，它由系统自动回收。
		*/
		Window* AddWindow(HWND hWnd, int w, int h, SizeType stw, SizeType sth) {
			Window* window = new Window(hWnd);
			window->SetSize(w, h, stw, sth);
			_sublist.push_back(window);
			return window;
		}

		/*
		* Layout函数，布局已经指派的控件对象。
		*   把指定控件布局在坐标(x, y)，w*h范围内。
		*/
		void Layout(int x, int y, int w, int h) {
			if (_boxType == HORIZONTAL)
				LayoutHorizontal(x, y, w, h);
			else
				LayoutVertical(x, y, w, h);
		}

	protected:
		/*
		* LayoutHorizontal函数，水平调整控件位置
		*/
		void LayoutHorizontal(int x, int y, int w, int h) {
			int objCount = (int)_sublist.size();
			int spaceWidth = w - _separatorSize * (objCount - 1);
			int residualWidth = spaceWidth;
			int objx, objy, objw, objh;
			std::vector<int> wlist;
			std::vector<int> fillIndexs;

			if (spaceWidth <= 0)
				spaceWidth = 0;

			/*计算每个对象宽度，填充类的为-1*/
			std::list<LayoutObject*>::iterator it = _sublist.begin();
			for (int index = 0; it != _sublist.end(); it++, index++) {
				LayoutObject* lo = (*it);

				if (lo->GetWST() == PIXEL) {
					objw = lo->GetW();
				}
				else if (lo->GetWST() == PERCENTAGE) {
					objw = (int)((double)spaceWidth * ((double)lo->GetW() / 100.0));
				}
				else if (lo->GetWST() == FILL) {
					objw = 0;
					fillIndexs.push_back(index);
				}
				else {
					objw = 0;
				}

				if (objw > residualWidth)
					objw = residualWidth;
				wlist.push_back(objw);
				residualWidth -= objw;
			}

			/*计算每个填充类对象宽度*/
			int fillIndexCount = (int)fillIndexs.size();
			if (fillIndexCount > 0) {
				int average = residualWidth / fillIndexCount;
				for (int i = 0; i < fillIndexCount; i++)
					wlist[fillIndexs[i]] = average;
				residualWidth -= average * fillIndexCount;
				for (int i = 0; i < residualWidth; i++) {
					int index = i % fillIndexCount;
					wlist[fillIndexs[index]] += 1;
				}
			}

			/*设置对象宽度*/
			objx = x;
			objy = y;
			it = _sublist.begin();
			for (int index = 0; it != _sublist.end(); it++, index++) {
				LayoutObject* lo = (*it);
				objw = wlist[index];

				if (lo->GetHST() == PIXEL) {
					objh = lo->GetH();
				}
				else if (lo->GetHST() == PERCENTAGE) {
					objh = (int)((double)h * ((double)lo->GetH() / 100.0));
				}
				else if (lo->GetHST() == FILL) {
					objh = h;
				}
				if (objh > h)
					objh = h;

				lo->Layout(objx, objy, objw, objh);

				objx += objw;
				objx += _separatorSize;
			}
		}

		/*
		* LayoutHorizontal函数，垂直调整控件位置
		*/
		void LayoutVertical(int x, int y, int w, int h) {
			int objCount = (int)_sublist.size();
			int spaceHeight = h - _separatorSize * (objCount - 1);
			int residualHeight = spaceHeight;
			int objx, objy, objw, objh;
			std::vector<int> hlist;
			std::vector<int> fillIndexs;

			if (spaceHeight <= 0)
				spaceHeight = 0;

			/*计算每个对象高度，填充类的为-1*/
			std::list<LayoutObject*>::iterator it = _sublist.begin();
			for (int index = 0; it != _sublist.end(); it++, index++) {
				LayoutObject* lo = (*it);

				if (lo->GetHST() == PIXEL) {
					objh = lo->GetH();
				}
				else if (lo->GetHST() == PERCENTAGE) {
					objh = (int)((double)spaceHeight * ((double)lo->GetH() / 100.0));
				}
				else if (lo->GetHST() == FILL) {
					objh = 0;
					fillIndexs.push_back(index);
				}
				else {
					objh = 0;
				}

				if (objh > residualHeight)
					objh = residualHeight;
				hlist.push_back(objh);
				residualHeight -= objh;
			}

			/*计算每个填充类对象的高度*/
			int fillIndexCount = (int)fillIndexs.size();
			if (fillIndexCount > 0) {
				int average = residualHeight / fillIndexCount;
				for (int i = 0; i < fillIndexCount; i++)
					hlist[fillIndexs[i]] = average;
				residualHeight -= average * fillIndexCount;
				for (int i = 0; i < residualHeight; i++) {
					int index = i % fillIndexCount;
					hlist[fillIndexs[index]] += 1;
				}
			}

			/*设置对象高度*/
			objx = x;
			objy = y;
			it = _sublist.begin();
			for (int index = 0; it != _sublist.end(); it++, index++) {
				LayoutObject* lo = (*it);
				objh = hlist[index];

				if (lo->GetWST() == PIXEL) {
					objw = lo->GetW();
				}
				else if (lo->GetWST() == PERCENTAGE) {
					objw = (int)((double)h * ((double)lo->GetW() / 100.0));
				}
				else if (lo->GetWST() == FILL) {
					objw = w;
				}
				else {
					objw = 0;
				}
				if (objw > w)
					objw = w;

				lo->Layout(objx, objy, objw, objh);

				objy += objh;
				objy += _separatorSize;
			}
		}

	protected:
		std::list<LayoutObject*> _sublist; /*对象列表*/
		int _separatorSize;                /*分隔条大小*/
		BoxType _boxType;                  /*盒子类型*/
	};

}

#endif

/*example
// 初始化布局
m_layout.SetType(Win32Layout::VERTICAL);

Win32Layout::Box* box1 = m_layout.AddHBox(0, 0, Win32Layout::FILL, Win32Layout::FILL);
box1->AddWindow(::GetDlgItem(m_hWnd, IDC_TREE1), 0, 0, Win32Layout::FILL, Win32Layout::FILL);
box1->AddWindow(::GetDlgItem(m_hWnd, IDC_LIST1), 0, 0, Win32Layout::FILL, Win32Layout::FILL);
box1->AddWindow(::GetDlgItem(m_hWnd, IDC_LIST2), 0, 0, Win32Layout::FILL, Win32Layout::FILL);

m_layout.AddWindow(::GetDlgItem(m_hWnd, IDC_EDIT1), 0, 100, Win32Layout::FILL, Win32Layout::PIXEL);

Win32Layout::Box* box2 = m_layout.AddHBox(0, 20, Win32Layout::FILL, Win32Layout::PIXEL);
box2->AddWindow(::GetDlgItem(m_hWnd, IDC_CHECK1), 50, 0, Win32Layout::PIXEL, Win32Layout::FILL);
box2->AddWindow(::GetDlgItem(m_hWnd, IDC_CHECK2), 50, 0, Win32Layout::PIXEL, Win32Layout::FILL);

Win32Layout::Box* box3 = m_layout.AddHBox(0, 20, Win32Layout::FILL, Win32Layout::PIXEL);
box3->AddWindow(::GetDlgItem(m_hWnd, IDC_EDIT2), 0, 0, Win32Layout::FILL, Win32Layout::FILL);
box3->AddWindow(::GetDlgItem(m_hWnd, IDC_BUTTON1), 80, 0, Win32Layout::PIXEL, Win32Layout::FILL);

WM_SIZE
m_layout.Layout(3, 3, cx - 6, cy - 6);
*/