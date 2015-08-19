#include "CmdAddPointGraph.h"
#include "CmdMediator.h"
#include "ColorFilter.h"
#include "DigitizeStateContext.h"
#include "DigitizeStatePointMatch.h"
#include "EngaugeAssert.h"
#include "EnumsToQt.h"
#include "GraphicsScene.h"
#include "GraphicsView.h"
#include "Logger.h"
#include "MainWindow.h"
#include "OrdinalGenerator.h"
#include <QCursor>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QImage>
#include <QPen>

const double Z_VALUE = 200.0;

DigitizeStatePointMatch::DigitizeStatePointMatch (DigitizeStateContext &context) :
  DigitizeStateAbstractBase (context),
  m_outline (0),
  m_candidatePoint (0)
{
}

DigitizeStatePointMatch::~DigitizeStatePointMatch ()
{
}

QString DigitizeStatePointMatch::activeCurve () const
{
  return context().mainWindow().selectedGraphCurve();
}

void DigitizeStatePointMatch::begin (DigitizeState /* previousState */)
{
  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStatePointMatch::begin";

  setCursor();
  context().setDragMode(QGraphicsView::NoDrag);
  context().mainWindow().updateViewsOfSettings(activeCurve ());

  // Add outline that will move with the cursor
  m_outline = new QGraphicsEllipseItem ();
  context().mainWindow().scene().addItem (m_outline);
  m_outline->setPen (QPen (Qt::black));
  m_outline->setVisible (true);
  m_outline->setZValue (Z_VALUE);
}

QCursor DigitizeStatePointMatch::cursor() const
{
  LOG4CPP_DEBUG_S ((*mainCat)) << "DigitizeStatePointMatch::cursor";

  return QCursor (Qt::ArrowCursor);
}

void DigitizeStatePointMatch::end ()
{
  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStatePointMatch::end";

  // Remove outline before leaving state
  ENGAUGE_CHECK_PTR (m_outline);
  context().mainWindow().scene().removeItem (m_outline);
  m_outline = 0;
}

void DigitizeStatePointMatch::handleCurveChange()
{
  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStatePointMatch::handleCurveChange";
}

void DigitizeStatePointMatch::handleKeyPress (Qt::Key key)
{
  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStatePointMatch::handleKeyPress key=" << QKeySequence (key).toString ().toLatin1 ().data ();
}

void DigitizeStatePointMatch::handleMouseMove (QPointF posScreen)
{
  LOG4CPP_DEBUG_S ((*mainCat)) << "DigitizeStatePointMatch::handleMouseMove";

  const DocumentModelPointMatch &modelPointMatch = context().cmdMediator().document().modelPointMatch();

  m_outline->setRect (posScreen.x() - modelPointMatch.maxPointSize() / 2.0,
                      posScreen.y() - modelPointMatch.maxPointSize() / 2.0,
                      modelPointMatch.maxPointSize(),
                      modelPointMatch.maxPointSize());

  const QImage &img = context().mainWindow().imageFiltered();

  ColorFilter filter;
  bool pixelShouldBeOn = filter.pixelFilteredIsOn (img,
                                                   posScreen.x(),
                                                   posScreen.y());

  QColor penColorIs = m_outline->pen().color();
  bool pixelIsOn = (penColorIs.red () != penColorIs.green()); // Considered on if not gray scale
  if (pixelShouldBeOn != pixelIsOn) {
    QColor penColorShouldBe (pixelShouldBeOn ? Qt::green : Qt::black);
    m_outline->setPen (QPen (penColorShouldBe));
  }
//    if (m_candidatePoint == 0) {
//
//      // Use color image with alpha so non-point pixels are transparent
//      QImage imgMask (modelPointMatch.maxPointSize(),
//                      modelPointMatch.maxPointSize(),
//                      QImage::Format_ARGB32);
//      imgMask.fill (Qt::transparent);
//
//      for (int xOffset = 0; xOffset < modelPointMatch.maxPointSize(); xOffset++) {
//        for (int yOffset = 0; yOffset < modelPointMatch.maxPointSize(); yOffset++) {
//
//          int x = posScreen.x() + xOffset;
//          int y = posScreen.y() + yOffset;
//          pixelIsOn = filter.pixelFilteredIsOn (img,
//                                                x,
//                                                y);
//          pixelIsOn = true;
//          if (pixelIsOn) {
//            imgMask.setPixel (xOffset,
//                              yOffset,
//                              qRgb (0, 255, 0));
//          }
//        }
//      }
//
//      QPixmap pixmap = QPixmap::fromImage (imgMask);
//      m_candidatePoint = new QGraphicsPixmapItem (pixmap,
//                                                  m_outline);
//      m_candidatePoint->setVisible (true);
//      m_candidatePoint->setZValue (Z_VALUE);
//    }
//    m_candidatePoint->setPos (posScreen.x(),
//                              posScreen.y());
//  } else {
//    if (m_candidatePoint != 0) {
//      context().mainWindow().scene().removeItem (m_candidatePoint);
//      m_candidatePoint = 0;
//    }
//  }
}

void DigitizeStatePointMatch::handleMousePress (QPointF /* posScreen */)
{
  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStatePointMatch::handleMousePress";
}

void DigitizeStatePointMatch::handleMouseRelease (QPointF posScreen)
{
  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStatePointMatch::handleMouseRelease";

//  // Create command to add point
//  OrdinalGenerator ordinalGenerator;
//  Document &document = context ().cmdMediator ().document ();
//  const Transformation &transformation = context ().mainWindow ().transformation();
//  QUndoCommand *cmd = new CmdAddPointGraph (context ().mainWindow(),
//                                            document,
//                                            context ().mainWindow().selectedGraphCurve(),
//                                            posScreen,
//                                            ordinalGenerator.generateCurvePointOrdinal(document,
//                                                                                       transformation,
//                                                                                       posScreen,
//                                                                                       activeCurve ()));
//  context().appendNewCmd(cmd);
}

QString DigitizeStatePointMatch::state() const
{
  return "DigitizeStatePointMatch";
}
