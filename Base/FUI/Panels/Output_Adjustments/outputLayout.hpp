#ifndef OUTPUT_LAYOUT_HPP
#define OUTPUT_LAYOUT_HPP

#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QString>

#include <CustomWidgets/fluoLine.hpp>
#include <CustomWidgets/fluoSlider.hpp>
#include <CustomWidgets/fluoSpinbox.hpp>
#include <CustomWidgets/fluoSpinboxDouble.hpp>
#include <CustomWidgets/fluoColoredLine.hpp>
#include <CustomWidgets/fluoToolButton.hpp>
#include <CustomWidgets/controller.hpp>

#include <cmath>
#include <vector>
#include <tuple>
#include <functional>

class OutputLayout : public QGridLayout
{
  Q_OBJECT

  signals:
    void sendRedGammaValue(std::any value);
    void sendGreenGammaValue(std::any value);
    void sendBlueGammaValue(std::any value);

    void sendRedLuminValue(int value);
    void sendGreenLuminValue(int value);
    void sendBlueLuminValue(int value);
    
    void sendRedEqlValue(std::any value);
    void sendGreenEqlValue(std::any value);
    void sendBlueEqlValue(std::any value);

  public slots:
    void onRedGSliderChanged() { sendRedGammaValue(redGammaSlider->value()); }
    void onGreenGSliderChanged() { sendGreenGammaValue(greenGammaSlider->value()); }
    void onBlueGSliderChanged() { sendBlueGammaValue(blueGammaSlider->value()); }
    void onRedGSpinChanged() { sendRedGammaValue(redGammaSpinbox->value()); }
    void onGreenGSpinChanged() { sendGreenGammaValue(greenGammaSpinbox->value()); }
    void onBlueGSpinChanged() { sendBlueGammaValue(blueGammaSpinbox->value()); }

    void onRedLSliderChanged() { sendRedLuminValue(redLuminSlider->value()); }
    void onGreenLSliderChanged() { sendGreenLuminValue(greenLuminSlider->value()); }
    void onBlueLSliderChanged() { sendBlueLuminValue(blueLuminSlider->value()); }
    void onRedLSpinChanged() { sendRedLuminValue(redLuminSpinbox->value()); }
    void onGreenLSpinChanged() { sendGreenLuminValue(greenLuminSpinbox->value()); }
    void onBlueLSpinChanged() { sendBlueLuminValue(blueLuminSpinbox->value()); }

    void onRedESliderChanged() { sendRedEqlValue(redEqlSlider->value()); }
    void onGreenESliderChanged() { sendGreenEqlValue(greenEqlSlider->value()); }
    void onBlueESliderChanged() { sendBlueEqlValue(blueEqlSlider->value()); }
    void onRedESpinChanged() { sendRedEqlValue(redEqlSpinbox->value()); }
    void onGreenESpinChanged() { sendGreenEqlValue(greenEqlSpinbox->value()); }
    void onBlueESpinChanged() { sendBlueEqlValue(blueEqlSpinbox->value()); }

  public:
    OutputLayout();

    template<typename T>
    void setRedGammaValue(T newVal) { redGammaController->setValues(newVal); }

    template<typename T>
    void setGreenGammaValue(T newVal) { greenGammaController->setValues(newVal); }

    template<typename T>
    void setBlueGammaValue(T newVal) { blueGammaController->setValues(newVal); }

    void setRedLuminValue(int newVal) { redLuminController->setValues(newVal); }
    void setGreenLuminValue(int newVal) { greenLuminController->setValues(newVal); }
    void setBlueLuminValue(int newVal) { blueLuminController->setValues(newVal); }

    template<typename T>
    void setRedEqlValue(T newVal) { redEqlController->setValues(newVal); }

    template<typename T>
    void setGreenEqlValue(T newVal) { greenEqlController->setValues(newVal); }

    template<typename T>
    void setBlueEqlValue(T newVal) { blueEqlController->setValues(newVal); }

  private:

    typedef void(OutputLayout::*outputFunc)();
    typedef void(FluoSlider::*sliderFunc)();
    typedef void(FluoSpinbox::*spinFunc)();
    typedef void(FluoSpinboxDouble::*dSpinFunc)();

    const int MAXCOLSIZE = 2;

    template<typename T>
    void addToWidget(T* widget,int row,int size)
    {
      int col = std::abs(size - MAXCOLSIZE);
      this->addWidget(widget,row,col,Qt::AlignHCenter);
    }

    template<typename T>
    void addRow(int row, T* widget)
    {
      this->addWidget(widget,row,MAXCOLSIZE,Qt::AlignHCenter);
    }

    template<typename Widget, typename... T>
    void addRow(int row, Widget *w,T*...t)
    {
      constexpr std::size_t n = sizeof...(T);
      addToWidget(w,row,n);
      addRow(row,t...);
    }

    template<typename T>
    void addSingle(int row, T *widget)
    {
      this->addWidget(widget,row,0,1,MAXCOLSIZE+1,Qt::AlignVCenter);
    }

    void constructLayout();
    void buildSliderConnections();
    void buildSpinboxConnections();
    void buildSpinboxDConnections();

    QLabel *gammaLabel     = new QLabel("Gamma");
    QLabel *luminanceLabel = new QLabel("Luminance");
    QLabel *equalLabel     = new QLabel ("Equalization");
    QLabel *redLabel       = new QLabel("Red");
    QLabel *greenLabel     = new QLabel("Green");
    QLabel *blueLabel      = new QLabel("Blue");

    FluoLine *redSepLine     = new FluoLine(QFrame::VLine);
    FluoLine *greenSepLine   = new FluoLine(QFrame::VLine);
    FluoLine *blueSepLine    = new FluoLine(QFrame::VLine);
    FluoLine *setDefaultLine = new FluoLine(QFrame::HLine);

    FluoToolButton *redChainButton   = new FluoToolButton(":/output-unlink.svg","",true,true,false);
    FluoToolButton *greenChainButton = new FluoToolButton(":/output-unlink.svg","",true,true,false);
    FluoToolButton *blueChainButton  = new FluoToolButton(":/output-unlink.svg","",true,true,false);

    QPushButton *redResetButton   = new QPushButton(QIcon(":/output-reset.svg")," Reset");
    QPushButton *greenResetButton = new QPushButton(QIcon(":/output-reset.svg")," Reset");
    QPushButton *blueResetButton  = new QPushButton(QIcon(":/output-reset.svg")," Reset");
    QPushButton *resetAllButton   = new QPushButton(QIcon(":/output-reset.svg")," Reset All");

    FluoColoredLine *redLine   = new FluoColoredLine(QFrame::HLine,"Red");
    FluoColoredLine *greenLine = new FluoColoredLine(QFrame::HLine,"Green");
    FluoColoredLine *blueLine  = new FluoColoredLine(QFrame::HLine,"Blue");

    FluoSlider *redGammaSlider   = new FluoSlider(Qt::Vertical,10,400);
    FluoSlider *greenGammaSlider = new FluoSlider(Qt::Vertical,10,400);
    FluoSlider *blueGammaSlider  = new FluoSlider(Qt::Vertical,10,400);

    FluoSlider *redLuminSlider   = new FluoSlider(Qt::Vertical,-256,256);
    FluoSlider *greenLuminSlider = new FluoSlider(Qt::Vertical,-256,256);
    FluoSlider *blueLuminSlider  = new FluoSlider(Qt::Vertical,-256,256);

    FluoSlider *redEqlSlider   = new FluoSlider(Qt::Vertical,0,100);
    FluoSlider *greenEqlSlider = new FluoSlider(Qt::Vertical,0,100);
    FluoSlider *blueEqlSlider  = new FluoSlider(Qt::Vertical,0,100);

    FluoSpinboxDouble *redGammaSpinbox   = new FluoSpinboxDouble(0.10,4.0,false);
    FluoSpinboxDouble *greenGammaSpinbox = new FluoSpinboxDouble(0.10,4.0,false);
    FluoSpinboxDouble *blueGammaSpinbox  = new FluoSpinboxDouble(0.10,4.0,false);

    FluoSpinbox *redLuminSpinbox   = new FluoSpinbox(-256,256,false);
    FluoSpinbox *greenLuminSpinbox = new FluoSpinbox(-256,256,false);
    FluoSpinbox *blueLuminSpinbox  = new FluoSpinbox(-256,256,false);

    FluoSpinboxDouble *redEqlSpinbox   = new FluoSpinboxDouble(0.0,1.0,false);
    FluoSpinboxDouble *greenEqlSpinbox = new FluoSpinboxDouble(0.0,1.0,false);
    FluoSpinboxDouble *blueEqlSpinbox  = new FluoSpinboxDouble(0.0,1.0,false);

    const std::vector<std::tuple<FluoSlider*, sliderFunc, outputFunc>> sliderConnections = {
      std::make_tuple(redGammaSlider, &FluoSlider::sliderReleased, &OutputLayout::onRedGSliderChanged),
      std::make_tuple(greenGammaSlider, &FluoSlider::sliderReleased, &OutputLayout::onGreenGSliderChanged),
      std::make_tuple(blueGammaSlider, &FluoSlider::sliderReleased, &OutputLayout::onBlueGSliderChanged),
      std::make_tuple(redLuminSlider, &FluoSlider::sliderReleased, &OutputLayout::onRedLSliderChanged),
      std::make_tuple(greenLuminSlider, &FluoSlider::sliderReleased, &OutputLayout::onGreenLSliderChanged),
      std::make_tuple(blueLuminSlider, &FluoSlider::sliderReleased, &OutputLayout::onBlueLSliderChanged),
      std::make_tuple(redEqlSlider, &FluoSlider::sliderReleased, &OutputLayout::onRedESliderChanged),
      std::make_tuple(greenEqlSlider, &FluoSlider::sliderReleased, &OutputLayout::onGreenESliderChanged),
      std::make_tuple(blueEqlSlider, &FluoSlider::sliderReleased, &OutputLayout::onBlueESliderChanged)
    };

    const std::vector<std::tuple<FluoSpinbox*, spinFunc, outputFunc>> spinConnections = {
      std::make_tuple(redLuminSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onRedLSpinChanged),
      std::make_tuple(greenLuminSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onGreenLSpinChanged),
      std::make_tuple(blueLuminSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onBlueLSpinChanged),
    };

    const std::vector<std::tuple<FluoSpinboxDouble*, dSpinFunc, outputFunc>> dSpinConnections = {
      std::make_tuple(redGammaSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onRedGSpinChanged),
      std::make_tuple(greenGammaSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onGreenGSpinChanged),
      std::make_tuple(blueGammaSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onBlueGSpinChanged),
      std::make_tuple(redEqlSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onRedESpinChanged),
      std::make_tuple(greenEqlSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onGreenESpinChanged),
      std::make_tuple(blueEqlSpinbox, &FluoSpinbox::editingFinished,&OutputLayout::onBlueESpinChanged),
    };

    const std::vector<std::function<void()>> rowFuncs = {
      std::bind(&OutputLayout::addRow<QLabel,QLabel,QLabel>,this,0,gammaLabel,luminanceLabel,equalLabel),
      std::bind(&OutputLayout::addRow<QLabel,FluoLine,FluoToolButton>,this,1,redLabel,redSepLine,redChainButton),
      std::bind(&OutputLayout::addSingle<FluoColoredLine>,this,2,redLine),
      std::bind(&OutputLayout::addRow<FluoSlider,FluoSlider,FluoSlider>,this,3,redGammaSlider,redLuminSlider,redEqlSlider),
      std::bind(&OutputLayout::addSingle<QPushButton>,this,4,redResetButton),
      std::bind(&OutputLayout::addRow<FluoSpinboxDouble,FluoSpinbox,FluoSpinboxDouble>,this,5,redGammaSpinbox,redLuminSpinbox,redEqlSpinbox),
      std::bind(&OutputLayout::addRow<QLabel,FluoLine,FluoToolButton>,this,6,greenLabel,greenSepLine,greenChainButton),
      std::bind(&OutputLayout::addSingle<FluoColoredLine>,this,7,greenLine),
      std::bind(&OutputLayout::addRow<FluoSlider,FluoSlider,FluoSlider>,this,8,greenGammaSlider,greenLuminSlider,greenEqlSlider),
      std::bind(&OutputLayout::addSingle<QPushButton>,this,9,greenResetButton),
      std::bind(&OutputLayout::addRow<FluoSpinboxDouble,FluoSpinbox,FluoSpinboxDouble>,this,10,greenGammaSpinbox,greenLuminSpinbox,greenEqlSpinbox),
      std::bind(&OutputLayout::addRow<QLabel,FluoLine,FluoToolButton>,this,11,blueLabel,blueSepLine,blueChainButton),
      std::bind(&OutputLayout::addSingle<FluoColoredLine>,this,12,blueLine),
      std::bind(&OutputLayout::addRow<FluoSlider,FluoSlider,FluoSlider>,this,13,blueGammaSlider,blueLuminSlider,blueEqlSlider),
      std::bind(&OutputLayout::addSingle<QPushButton>,this,14,blueResetButton),
      std::bind(&OutputLayout::addRow<FluoSpinboxDouble,FluoSpinbox,FluoSpinboxDouble>,this,15,blueGammaSpinbox,blueLuminSpinbox,blueEqlSpinbox),
      std::bind(&OutputLayout::addSingle<FluoLine>,this,16,setDefaultLine),
      std::bind(&OutputLayout::addSingle<QPushButton>,this,17,resetAllButton)
    };

    Controller<FluoSlider, FluoSpinboxDouble> *redGammaController =
      new Controller<FluoSlider, FluoSpinboxDouble>(*redGammaSlider, *redGammaSpinbox);
    Controller<FluoSlider, FluoSpinboxDouble> *greenGammaController =
        new Controller<FluoSlider, FluoSpinboxDouble>(*greenGammaSlider, *greenGammaSpinbox);
    Controller<FluoSlider, FluoSpinboxDouble> *blueGammaController =
        new Controller<FluoSlider, FluoSpinboxDouble>(*blueGammaSlider, *blueGammaSpinbox);
    Controller<FluoSlider, FluoSpinbox> *redLuminController =
        new Controller<FluoSlider, FluoSpinbox>(*redLuminSlider, *redLuminSpinbox);
    Controller<FluoSlider,FluoSpinbox> *greenLuminController =
       new Controller<FluoSlider,FluoSpinbox>(*greenLuminSlider, *greenLuminSpinbox);
    Controller<FluoSlider,FluoSpinbox> *blueLuminController = 
      new Controller<FluoSlider,FluoSpinbox>(*blueLuminSlider,*blueLuminSpinbox);
    Controller<FluoSlider,FluoSpinboxDouble> *redEqlController =
      new Controller<FluoSlider,FluoSpinboxDouble>(*redEqlSlider,*redEqlSpinbox);
    Controller<FluoSlider,FluoSpinboxDouble> *greenEqlController = 
      new Controller<FluoSlider,FluoSpinboxDouble>(*greenEqlSlider,*greenEqlSpinbox);
    Controller<FluoSlider,FluoSpinboxDouble> *blueEqlController = 
      new Controller<FluoSlider,FluoSpinboxDouble>(*blueEqlSlider,*blueEqlSpinbox);
};

#endif 
