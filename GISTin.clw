; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CGISTinView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "GISTin.h"
LastPage=0

ClassCount=7
Class1=CGISTinApp
Class2=CGISTinDoc
Class3=CGISTinView
Class4=CMainFrame

ResourceCount=3
Resource1=IDD_ABOUTBOX
Class5=CAboutDlg
Resource2=IDR_MAINFRAME
Class6=CGridDlg
Class7=CThresholdDlg
Resource3=IDD_GRID_DLG

[CLS:CGISTinApp]
Type=0
HeaderFile=GISTin.h
ImplementationFile=GISTin.cpp
Filter=N

[CLS:CGISTinDoc]
Type=0
HeaderFile=GISTinDoc.h
ImplementationFile=GISTinDoc.cpp
Filter=N

[CLS:CGISTinView]
Type=0
HeaderFile=GISTinView.h
ImplementationFile=GISTinView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC
LastObject=ID_DISPLAY_BINARY_TREE


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T
LastObject=ID_DIVIDE_GENERATE_TIN




[CLS:CAboutDlg]
Type=0
HeaderFile=GISTin.cpp
ImplementationFile=GISTin.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_LOAD_POINT_FILE
Command2=ID_GENERATE_DELAUNAY
Command3=ID_DIVIDE_GENERATE_TIN
Command4=D_DIVIDE_GENERATE_TIN2
Command5=ID_PARALLEL_GENERATE_TIN
Command6=ID_PARALLEL_GENERATE_TIN2
Command7=ID_ZOOM_IN
Command8=ID_ZOOM_OUT
Command9=ID_PAN
Command10=ID_GLOBE
Command11=ID_SELECT
Command12=ID_DISPLAY_GENERATE_DELAUNAY
Command13=ID_DISPLAY_MERGE_DELAUNAY
Command14=ID_DISPLAY_BINARY_TREE
Command15=ID_DISPLAY_GRID
CommandCount=15

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_GLOBE
Command2=ID_ZOOM_IN
Command3=ID_ZOOM_OUT
Command4=ID_PAN
Command5=ID_SELECT
CommandCount=5

[DLG:IDD_GRID_DLG]
Type=1
Class=CGridDlg
ControlCount=10
Control1=IDC_EDIT1,edit,1350631553
Control2=IDC_EDIT2,edit,1350631553
Control3=IDOK,button,1342242817
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT3,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT4,edit,1350631552

[CLS:CGridDlg]
Type=0
HeaderFile=GridDlg.h
ImplementationFile=GridDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CGridDlg

[CLS:CThresholdDlg]
Type=0
HeaderFile=ThresholdDlg.h
ImplementationFile=ThresholdDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC

