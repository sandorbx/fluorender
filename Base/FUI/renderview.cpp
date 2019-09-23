#include "renderview.hpp"


/*
 * This constructor creates a new QMainWindow for a dock widget to
 * sit on top of. Toolbars are then created with specified orientations.
 * A new OpenGLWidget is then create and made as the central widget
 * of my QMainWindow. The toolbars are then added in their appropriate
 * locations. If there are no features, the dock widget is set to
 * no dock widget features, and the boolean isMainWindow is set to true.
 *
 * The newly created QMainWindow then adds the dock widget and all of
 * the features that comes with it.
 */
RenderView::RenderView(QWidget *parent, bool hasFeatures, int renderNumber) : QMainWindow (parent)
{
    auto newBaseWindow = std::make_unique<QMainWindow>(new QMainWindow);
    auto baseDockWidget = std::make_unique<QDockWidget>(new QDockWidget);
    auto leftToolBar = genToolProp(Qt::Vertical);
    auto topToolBar = genToolProp(Qt::Horizontal);
    auto rightToolBar = genToolProp(Qt::Vertical);
    auto bottomToolBar = genToolProp(Qt::Horizontal);


    // TODO: Instead of creating a new OpenGLWidget, I need to create a class
    //       that inherits an OpenGLWidget/OpenGLWindow.
    //auto newRenderView = std::make_unique<QOpenGLWidget>(new QOpenGLWidget);

    std::unique_ptr<FluoGLWidget> newRenderView;
    //auto newRenderView = std::make_unique<FluoGLWidget>(new FluoGLWidget);

    if(!hasFeatures)
    {
        baseDockWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
        isMainWindow = true;
        newRenderView = std::make_unique<FluoGLWidget>(true);
    }
    else
    {
        newRenderView = std::make_unique<FluoGLWidget>();
    }

    populateLeftToolBar(leftToolBar);
    populateRightToolBar(rightToolBar);
    populateTopToolBar(topToolBar);
    populateBottomToolBar(bottomToolBar);

    newBaseWindow->setCentralWidget(QWidget::createWindowContainer(newRenderView.release()));
    newBaseWindow->addToolBar(Qt::LeftToolBarArea,leftToolBar.release());
    newBaseWindow->addToolBar(Qt::TopToolBarArea,topToolBar.release());
    newBaseWindow->addToolBar(Qt::RightToolBarArea, rightToolBar.release());
    newBaseWindow->addToolBar(Qt::BottomToolBarArea,bottomToolBar.release());

    baseDockWidget->setWidget(newBaseWindow.release());



    this->setWindowFlags(Qt::Widget);
    baseDockWidget->setWindowTitle("Renderview " + QString::number(renderNumber));
    this->addDockWidget(Qt::RightDockWidgetArea, baseDockWidget.release());
}

void RenderView::populateLeftToolBar(std::unique_ptr<QToolBar> &leftToolBar)
{
  auto newSlider = std::make_unique<QSlider>(new QSlider);
  auto newSpinBox = std::make_unique<QDoubleSpinBox>(new QDoubleSpinBox);

  newSlider->setOrientation(Qt::Vertical);

  // TODO: Find out how to not get this to crash, the slider needs to be
  //       in the middle of the toolbar.

  //QLayout *sliderLayout = newSlider->layout();
  //sliderLayout->setAlignment(newSlider.release(),Qt::AlignHCenter);
  leftToolBar->addWidget(newSlider.release());
  leftToolBar->addWidget(newSpinBox.release());
}

void RenderView::populateRightToolBar(std::unique_ptr<QToolBar> &rightToolBar)
{

}

void RenderView::populateTopToolBar(std::unique_ptr<QToolBar> &topToolBar)
{

}

void RenderView::populateBottomToolBar(std::unique_ptr<QToolBar> &bottomToolBar)
{

}

/*
 * This creates my toolbars, the orientation is set, and is returned.
 */
std::unique_ptr<QToolBar> RenderView::genToolProp(Qt::Orientation ori)
{
    auto newToolBar = std::make_unique<QToolBar>(new QToolBar);

    newToolBar->setMovable(false);
    newToolBar->setOrientation(ori);
    newToolBar->setStyleSheet("QToolBar {background: rgb(222,225,232)}");

    if(ori == Qt::Horizontal)
        newToolBar->setFixedHeight(45);
    else
        newToolBar->setFixedWidth(45);

    return newToolBar;
}

// This is simply for debugging.
bool RenderView::getMainWindowStatus()
{
    return isMainWindow;
}

