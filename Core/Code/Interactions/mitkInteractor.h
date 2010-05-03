/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef INTERACTOR_H_HEADER_INCLUDED
#define INTERACTOR_H_HEADER_INCLUDED

#include "mitkCommon.h"
#include "mitkStateMachine.h"
#include "mitkGeometry3D.h"

#include <string>

namespace mitk {

class DataNode;
class BaseData;

//##Documentation
//## @brief Interface for an Interactor.
//##
//## The Interactor is held with a SmartPointer by a DataNode
//## and holds its Node with a Pointer. That way Smartpointer doesn't build a circle.
//## Different Modes: In order to not send Events to all StateMachines, a StateMachine can be
//## in three different modes:
//## DESELECTED: this statemachine doesn't wait for an event
//## SELECTED: this statemachine just has handled an event and waits for the next one
//## SUBSELECTED: depricate; was used for hierarchical statemachines before.
//## Guidelines for the modevalues: Selected if the coresponding data is selected, deselected if deselect of data.
//##
//## In moving the machine is selected. After a new insert the machine is selected, since the data is also selected
//## In method ExecuteAction(..) the different actions are divided up through switch/case statements. Each block has to check
//## the appropriate type of event to process the actions. Especially in guarding states (a state, that checks certain conditions (e.g. is picked)
//## the according Event must be called to continue in states. No return false here!
//## @ingroup Interaction
class MITK_CORE_EXPORT Interactor : public StateMachine
{
public:
    mitkClassMacro(Interactor, StateMachine);

    /**
    * @brief NewMacro with two parameters for calling itk::Lightobject::New(..) method
    **/
    mitkNewMacro2Param(Self, const char*, DataNode*);

  //##Documentation
  //##@brief Enumeration of the different modes an Interactor can be into.
  //## See class documentation for further details
  enum SMMode
  {
    SMDESELECTED = 0,
    SMSELECTED,
    SMSUBSELECTED
  };

  typedef SMMode ModeType;

  //##Documentation
  //## @brief Get the Mode of the Interactor. Use enum SMMode for return parameter
  SMMode GetMode() const;

  //##Documentation
  //## @brief Check the interaction mode
  bool IsNotSelected() const;

  //##Documentation
  //## @brief Check the interaction mode
  bool IsSelected() const;

  //##Documentation
  //## @brief calculates how good the data, this statemachine handles, is hit by the event.
  //##
  //## Returns a value between 0 and 1
  //## where 0 represents not responsible and 1 represents definitive responsible!
  //## Standard function to override if needed.
  //## (Used by GlobalInteraction to decide which DESELECTED statemachine to send the event to.)
  virtual float CanHandleEvent(StateEvent const* stateEvent) const;

  /**
  * @brief Updates the current TimeStep according to the associated data and calls Superclass::HandleEvent()
  **/
  bool HandleEvent(StateEvent const* stateEvent);

  static const std::string XML_NODE_NAME;

protected:
  /**
  * @brief Constructor
  * @param dataTreeNode is the node, this Interactor is connected to
  * @param type is the type of StateMachine like declared in the XML-Configure-File
  *
  * Interactor connects itself to the DataNode-Interactor-pointer through call of SetInteractor(this)
  **/
  Interactor(const char * type, DataNode* dataTreeNode);

  /**
  * @brief Destructor
  **/
  ~Interactor(){}

  bool OnModeSelect(Action* action, StateEvent const*);
  bool OnModeDeselect(Action* action, StateEvent const*);
  bool OnModeSubSelect(Action* action, StateEvent const*);

  //##Documentation
  //## @brief adds the handling of Operations used for mode change. Unrecognized Operations are send to Superclass.
  virtual void ExecuteOperation(Operation* operation);

  //##Documentation
  virtual const std::string& GetXMLNodeName() const;

  //##Documentation
  //## @brief creates a ModeOperation with the transmitted mode and sends it to this. Undo supported!
  void CreateModeOperation(ModeType mode);

  //##Documentation
  //## @brief convenience method for accessing the data contained in the
  //## node to which this interactor is associated to
  BaseData* GetData() const;


  //##Documentation
  //## @brief Used by friend class DataNode
  virtual void SetDataNode( DataNode* dataTreeNode );

  /**
  * @brief Derived from superclass to also check if enough timesteps are instantiated in m_CurrentStateVector
  * The number of timesteps is read from the dedicated data.
  * @param[in] timeStep The timeStep that the statemachine has to be set to
  **/
  virtual void UpdateTimeStep(unsigned int timeStep);

  //##Documentation
  //## @brief Pointer to the data, this object handles the Interaction for
  DataNode* m_DataNode;

  //##Documentation
  //## @brief Mode of Selection
  ModeType m_Mode;

  friend class DataNode;
};

}//namespace mitk
#endif /* INTERACTOR_H_HEADER_INCLUDED */


