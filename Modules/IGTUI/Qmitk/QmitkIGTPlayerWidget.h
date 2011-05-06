/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date: 2009-05-12 19:14:45 +0200 (Di, 12 Mai 2009) $
Version:   $Revision: 1.12 $

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef QmitkIGTPlayerWidget_H
#define QmitkIGTPlayerWidget_H

//QT headers
#include <QWidget>

//mitk headers
#include "MitkIGTUIExports.h"
#include "mitkNavigationTool.h"
#include "mitkNavigationDataPlayer.h"
#include <mitkTimeStamp.h>

#include <mitkPointSet.h>



//ui header
#include "ui_QmitkIGTPlayerWidgetControls.h"

/** Documentation:
*   \brief GUI to access the IGT Player. 
*   User must specify the file name where the input xml-file is located. The NavigationDatas from the xml-file can be 
*   played in normal mode or in PointSet mode. 
*   
*   In normal mode the player updates the NavigationDatas every 100ms (can be changed in SetUpdateRate()) and returns
*   them when GetNavigationDatas() is called.
*   In PointSet mode the player generates a PointSet with all NavigationDatas from the xml-file. So the playback is
*   performed on this ND PointSet.
*
*   \ingroup IGTUI
*/
class MitkIGTUI_EXPORT QmitkIGTPlayerWidget : public QWidget
{
  Q_OBJECT

public:
  static const std::string VIEW_ID;

  /*!  
  \brief default constructor  
  */ 
  QmitkIGTPlayerWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);    

  /*!  
  \brief default deconstructor  
  */ 
  ~QmitkIGTPlayerWidget();

  /*!  
  \brief Sets the player for this player widget 
  */ 
  void SetPlayer(mitk::NavigationDataPlayer::Pointer player);

  /*!  
  \brief Returns the playing timer of this widget  
  */ 
  QTimer* GetPlayingTimer();

  /*!  
  \brief Returns the current playback NavigationDatas from the xml-file
  */ 
  const std::vector<mitk::NavigationData::Pointer> GetNavigationDatas();

  /*!  
  \brief Returns a PointSet of the current NavigationDatas for all recorded tools.
  */
  const mitk::PointSet::Pointer GetNavigationDatasPointSet();

  /*!  
  \brief Returns a PointType of the current NavigationData for a specific tool with the given index.
  */
  const mitk::PointSet::PointType GetNavigationDataPoint(unsigned int index);


  /*!  
  \brief Sets the update rate of this widget's playing timer
  */ 
  void SetUpdateRate(unsigned int msecs);


  /*!
  \brief Returns the number of different tools from the current playing stream.
  *
  * Retuns 0 if playback file is invalid.
  */
  unsigned int GetNumberOfTools();

  /*!  
  \brief Stops the playback
  */ 
  void StopPlaying();

  /*!
  \brief Sets the given tool names list to the trajectory select combobox.
  */
  void SetTrajectoryNames(const QStringList toolNames);

  /*!
  \brief Returns the current resolution value from the resolution spinbox.
  */
  int GetResolution();
  
  /*!
  \brief Clears all items in the trajectory selection combobox.
  */
  void ClearTrajectorySelectCombobox();

  /*!
  \brief Returns whether spline mode checkbox is selected.
  */
  bool IsTrajectoryInSplineMode();


signals:
  /*!  
  \brief This signal is emitted when the player starts the playback.
  */ 
  void SignalPlayingStarted();
  /*!  
  \brief This signal is emitted when the player resumes after a pause.
  */ 
  void SignalPlayingResumed();
  /*!  
  \brief This signal is emitted when the player stops.
  */ 
  void SignalPlayingStopped();
  /*!  
  \brief This signal is emitted when the player is paused.
  */ 
  void SignalPlayingPaused();
  /*!  
  \brief This signal is emitted when the player reaches the end of the playback.
  */ 
  void SignalPlayingEnded();

  /*!  
  \brief This signal is emitted every time the player updated the NavigationDatas.
  */ 
  void SignalPlayerUpdated();

  /*!  
  \brief This signal is emitted if the input file for the replay was changed.
  */ 
  void SignalInputFileChanged();

  /*!  
  \brief This signal is emitted if the index of the current selected trajectory select combobox item changes.
  */
  void SignalCurrentTrajectoryChanged(int index);

  /*!  
  \brief This signal is emitted if the spline mode checkbox is toggled or untoggled.
  */
  void SignalSplineModeToggled(bool checked);




  protected slots:   
    /*!  
    \brief Starts or pauses the playback
    */ 
    void OnPlayButtonClicked(bool toggled);    
    /*!  
    \brief Updates the playback data
    */ 
    void OnPlaying();   
    /*!  
    \brief Stops the playback
    */  
    void OnStopPlaying();
    /*!  
    \brief Updates the input filename
    */ 
    void SetInputFileName(const QString& inputFileName);
    /*!  
    \brief Opens file open dialog for searching the input file
    */ 
    void OnSelectPressed();
    /*!  
    \brief Stops the playback
    */ 
    void OnGoToEnd();
    /*!  
    \brief Stops the playback and resets the player to the beginning
    */ 
    void OnGoToBegin();


protected:

  /// \brief Creation of the connections
  virtual void CreateConnections();


  virtual void CreateQtPartControl(QWidget *parent);

  /*!  
  \brief Checks if an imput file with the set filename exists
  */ 
  bool CheckInputFileValid();

  /*!  
  \brief Sets all LCD numbers to 0
  */ 
  void ResetLCDNumbers();

  Ui::QmitkIGTPlayerWidgetControls* m_Controls;

  mitk::NavigationDataPlayer::Pointer m_Player; ///< plays NDs from a XML file

  QString m_CmpFilename; ///< filename of the input file
  QTimer* m_PlayingTimer; ///< update timer

  mitk::NavigationData::TimeStampType m_StartTime; ///< start time of playback needed for time display

};
#endif