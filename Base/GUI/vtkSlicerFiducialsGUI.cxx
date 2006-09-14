#include "vtkObject.h"
#include "vtkObjectFactory.h"
#include "vtkCommand.h"
#include "vtkKWWidget.h"
#include "vtkSlicerModelsGUI.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerVisibilityIcons.h"

#include "vtkKWMessage.h"
#include "vtkKWMultiColumnList.h"
#include "vtkKWMultiColumnListWithScrollbars.h"

#include "vtkKWFrameWithLabel.h"

#include "vtkKWMenu.h"
#include "vtkKWMenuButton.h"

#include "vtkSlicerFiducialsGUI.h"

//---------------------------------------------------------------------------
vtkStandardNewMacro (vtkSlicerFiducialsGUI );
vtkCxxRevisionMacro ( vtkSlicerFiducialsGUI, "$Revision: 1.0 $");


//---------------------------------------------------------------------------
vtkSlicerFiducialsGUI::vtkSlicerFiducialsGUI ( )
{
    this->Logic = NULL;
    this->FiducialListSelectorWidget = NULL;
    this->FiducialListNodeID = NULL; // "(none)";

    this->AddFiducialButton = NULL;

    this->VisibilityToggle = NULL;
    this->VisibilityIcons = NULL;

    this->ListColorButton = NULL;
    this->ListSelectedColorButton = NULL;
    this->ListSymbolScale = NULL;
    this->ListTextScale = NULL;
    this->ListOpacity = NULL;
    
    this->MultiColumnList = NULL;
    this->NumberOfColumns = 9;
}


//---------------------------------------------------------------------------
vtkSlicerFiducialsGUI::~vtkSlicerFiducialsGUI ( )
{

    this->SetModuleLogic ( NULL );

    if (this->FiducialListSelectorWidget)
    {
        this->FiducialListSelectorWidget->SetParent(NULL);
        this->FiducialListSelectorWidget->Delete();
        this->FiducialListSelectorWidget = NULL;
    }
     
    if (this->AddFiducialButton ) {
        this->AddFiducialButton->SetParent (NULL );
        this->AddFiducialButton->Delete ( );
        this->AddFiducialButton = NULL;
    }

    if (this->VisibilityToggle) {
        this->VisibilityToggle->SetParent(NULL);
        this->VisibilityToggle->Delete();
        this->VisibilityToggle = NULL;
    }

    if ( this->VisibilityIcons ) {
        this->VisibilityIcons->Delete  ( );
        this->VisibilityIcons = NULL;
    }

    if (this->ListColorButton) {
        this->ListColorButton->SetParent(NULL);
        this->ListColorButton->Delete();
        this->ListColorButton = NULL;
    }

    if (this->ListSelectedColorButton) {
        this->ListSelectedColorButton->SetParent(NULL);
        this->ListSelectedColorButton->Delete();
        this->ListSelectedColorButton = NULL;
    }

    if (this->ListSymbolScale) {
        this->ListSymbolScale->SetParent(NULL);
        this->ListSymbolScale->Delete();
        this->ListSymbolScale = NULL;
    }

    if (this->ListTextScale) {
        this->ListTextScale->SetParent(NULL);
        this->ListTextScale->Delete();
        this->ListTextScale = NULL;
    }

    if (this->ListOpacity) {
        this->ListOpacity->SetParent(NULL);
        this->ListOpacity->Delete();
        this->ListOpacity = NULL;
    }

    if (this->MultiColumnList) {
      this->MultiColumnList->SetParent(NULL);
      this->MultiColumnList->Delete();
      this->MultiColumnList = NULL;
    }

    //this->SetFiducialListNodeID("(none)");
    this->SetFiducialListNodeID(NULL);
}


//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::PrintSelf ( ostream& os, vtkIndent indent )
{
    this->vtkObject::PrintSelf ( os, indent );

    os << indent << "SlicerFiducialsGUI: " << this->GetClassName ( ) << "\n";
    os << indent << "Logic: " << this->GetLogic ( ) << "\n";
    // print widgets?
}



//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::RemoveGUIObservers ( )
{
    vtkDebugMacro("vtkSlicerFiducialsGUI: RemoveGUIObservers\n");
    this->FiducialListSelectorWidget->RemoveObservers(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->AddFiducialButton->RemoveObservers ( vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->VisibilityToggle->RemoveObservers (vtkKWPushButton::InvokedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ListColorButton->RemoveObservers (vtkKWChangeColorButton::ColorChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ListSelectedColorButton->RemoveObservers (vtkKWChangeColorButton::ColorChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ListSymbolScale->RemoveObservers (vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ListTextScale->RemoveObservers (vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ListOpacity->RemoveObservers (vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    
    this->RemoveObservers (vtkSlicerFiducialsGUI::FiducialListIDModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
}


//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::AddGUIObservers ( )
{
    vtkDebugMacro("vtkSlicerFiducialsGUI: AddGUIObservers\n");
    this->FiducialListSelectorWidget->AddObserver(vtkSlicerNodeSelectorWidget::NodeSelectedEvent, (vtkCommand *)this->GUICallbackCommand );  
    this->AddFiducialButton->AddObserver ( vtkKWPushButton::InvokedEvent,  (vtkCommand *)this->GUICallbackCommand );

    this->VisibilityToggle->AddObserver (vtkKWPushButton::InvokedEvent,  (vtkCommand *)this->GUICallbackCommand );
    this->ListColorButton->AddObserver (vtkKWChangeColorButton::ColorChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ListSelectedColorButton->AddObserver (vtkKWChangeColorButton::ColorChangedEvent, (vtkCommand *)this->GUICallbackCommand );
    this->ListSymbolScale->AddObserver (vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ListTextScale->AddObserver (vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    this->ListOpacity->AddObserver (vtkKWScale::ScaleValueChangedEvent, (vtkCommand *)this->GUICallbackCommand);
    
    this->AddObserver(vtkSlicerFiducialsGUI::FiducialListIDModifiedEvent, (vtkCommand *)this->GUICallbackCommand);
}



//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::ProcessGUIEvents ( vtkObject *caller,
                                            unsigned long event, void *callData )
{
    // process fiducial list node selector events
    vtkSlicerNodeSelectorWidget *fidListSelector = 
        vtkSlicerNodeSelectorWidget::SafeDownCast(caller);
    if (fidListSelector == this->FiducialListSelectorWidget &&
        event == vtkSlicerNodeSelectorWidget::NodeSelectedEvent )
    {
        vtkDebugMacro("vtkSlicerFiducialsGUI: ProcessGUIEvent Node Selector Event: " << event << ".\n");
        vtkMRMLFiducialListNode *fidList =
            vtkMRMLFiducialListNode::SafeDownCast(this->FiducialListSelectorWidget->GetSelected());
        if (fidList != NULL)
        {
            this->SetFiducialListNode(fidList);
        }
        return;
    }
    vtkMRMLFiducialListNode *fidList =
        vtkMRMLFiducialListNode::SafeDownCast(caller);
    if (fidList == this->MRMLScene->GetNodeByID(this->GetFiducialListNodeID()) &&
        event == vtkCommand::ModifiedEvent)
    {
//        std::cout << "vtkSlicerFiducialsGUI: ProcessGUIEvent list node modified event " << event << endl;
        return;
    }
    
  // get the currently displayed list
  
  // is there one list?
  vtkMRMLFiducialListNode *activeFiducialListNode = (vtkMRMLFiducialListNode *)this->MRMLScene->GetNodeByID(this->GetFiducialListNodeID());
  if (activeFiducialListNode == NULL)
  {
      std::cerr << "ERROR: No Fiducial List, add one first!\n";
      return;
  }
  // save state for undo
  this->MRMLScene->SaveStateForUndo(activeFiducialListNode);

  vtkKWPushButton *button = vtkKWPushButton::SafeDownCast(caller);
  if (button == this->AddFiducialButton  && event ==  vtkKWPushButton::InvokedEvent)
    {
        vtkDebugMacro("vtkSlicerFiducialsGUI: ProcessGUIEvent: Add Fiducial Button event: " << event << ".\n");
       // save state for undo
       this->MRMLScene->SaveStateForUndo();

       // add a fiducial, get the index of the new fiducial
       int modelIndex = activeFiducialListNode->AddFiducial();
       if ( modelIndex < 0 ) 
       {
           // TODO: generate an error...
           std::cerr << "ERROR adding a new fiducial point\n";
           return;
       }
    }
  if (button == this->GetVisibilityToggle()  && event ==  vtkKWPushButton::InvokedEvent)
    {
        vtkDebugMacro("vtkSlicerFiducialsGUI: ProcessGUIEvent: Visibility button event: " << event << ".\n");
       // change the visibility
        activeFiducialListNode->SetVisibility( ! activeFiducialListNode->GetVisibility());
        // update the icon via  process mrml event that should get pushed
        //this->ProcessMRMLEvents(caller, event, callData); 

    }

  // list colour
  vtkKWChangeColorButton *colorButton = vtkKWChangeColorButton::SafeDownCast(caller);
  if (colorButton == this->ListColorButton && event == vtkKWChangeColorButton::ColorChangedEvent)
  {
      vtkDebugMacro("ProcessGUIEvents: list colour button change event\n");
       // change the colour
      activeFiducialListNode->SetColor(this->ListColorButton->GetColor());
        // this->ProcessMRMLEvents(caller, event, callData); 

  }
  if (colorButton == this->ListSelectedColorButton && event == vtkKWChangeColorButton::ColorChangedEvent)
  {
      vtkDebugMacro("ProcessGUIEvents: list selected colour button change event\n");
      // change the selected colour
      activeFiducialListNode->SetSelectedColor(this->ListSelectedColorButton->GetColor());
  }
  
  // list symbol and text sizes
  vtkKWScaleWithEntry *scale = vtkKWScaleWithEntry::SafeDownCast(caller);
  if (scale == this->ListSymbolScale && event == vtkKWScale::ScaleValueChangedEvent)
  {
     activeFiducialListNode->SetSymbolScale(this->ListSymbolScale->GetValue());
      // this->ProcessMRMLEvents(caller, event, callData);                                                     
  }
  else if (scale == this->ListTextScale && event == vtkKWScale::ScaleValueChangedEvent)
  {
      activeFiducialListNode->SetTextScale(this->ListTextScale->GetValue());
      // this->ProcessMRMLEvents(caller, event, callData); 
  }
  else if (scale == this->ListOpacity && event == vtkKWScale::ScaleValueChangedEvent)
  {
      activeFiducialListNode->SetOpacity(this->ListOpacity->GetValue());
  }
  return;
}

//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::ProcessLogicEvents ( vtkObject *caller,
                                              unsigned long event, void *callData )
{
    // std::cout << "vtkSlicerFiducialsGUI: Process Logic Events " << event << ".\n";
    // Fill in
}

//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::ProcessMRMLEvents ( vtkObject *caller,
                                             unsigned long event, void *callData )
{    
    vtkDebugMacro("vtkSlicerFiducialsGUI::ProcessMRMLEvents: event = " << event << ".\n");
    
    if (event == vtkCommand::WidgetValueChangedEvent)
    {
        vtkDebugMacro("got a widget value changed event... the list node was changed.\n");
    }

    vtkMRMLFiducialListNode *node = vtkMRMLFiducialListNode::SafeDownCast(caller);
    vtkMRMLFiducialListNode *activeFiducialListNode = (vtkMRMLFiducialListNode *)this->MRMLScene->GetNodeByID(this->GetFiducialListNodeID());
    
    if (node == activeFiducialListNode && event == vtkCommand::ModifiedEvent)
    {
        vtkDebugMacro("\tmodified event on the fiducial list node.\n");
        if (node == NULL)
        {
            vtkDebugMacro("\tBUT: the node is null\n");
            return;
        }
        vtkDebugMacro("\t\tUpdating the GUI\n");
        // update the table
        SetGUIFromList(activeFiducialListNode);
        return;
    }
    if (node == activeFiducialListNode && event == vtkMRMLFiducialListNode::FiducialModifiedEvent)
    {
        vtkDebugMacro("\tfiducial modified event on the active fiducial list.");
        if (node == NULL)
        {
            return;
        }
        SetGUIFromList(activeFiducialListNode);
        return;
    }
    
    if (node == activeFiducialListNode && event == vtkMRMLFiducialListNode::DisplayModifiedEvent)
    {
        vtkDebugMacro("vtkSlicerFiducialsGUI::ProcessMRMLEvents: DisplayModified event on the fiducial list node...\n");
    }
    if (node == vtkMRMLFiducialListNode::SafeDownCast(this->FiducialListSelectorWidget->GetSelected()) && event == vtkCommand::ModifiedEvent)
    {
        vtkDebugMacro("\tmodified event on the fiducial list selected node.\n");
        if (activeFiducialListNode !=  vtkMRMLFiducialListNode::SafeDownCast(this->FiducialListSelectorWidget->GetSelected()))
        {
            // select it first off
            this->SetFiducialListNode(vtkMRMLFiducialListNode::SafeDownCast(this->FiducialListSelectorWidget->GetSelected()));
        }
        SetGUIFromList(activeFiducialListNode);
        return;        
    }    


    vtkDebugMacro("vtkSlicerFiducialsGUI: Done processing mrml events...");
//    std::cerr << "vtkSlicerFiducialsGUI::ProcessMRMLEvents  mismatched
//    caller and event (" << event << ")\n";
}

//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::SetGUIFromList(vtkMRMLFiducialListNode * activeFiducialListNode)
{
    // clear out the multi column list box and fill it in with the
    // new list
        this->MultiColumnList->GetWidget()->DeleteAllRows();

        // add rows for each point
        int numPoints = activeFiducialListNode->GetNumberOfFiducials();
        float *xyz;
        float *wxyz;
        for (int row = 0; row < numPoints; row++)
        {
            vtkDebugMacro("SetGUIFromList: Adding point " << row << " to the table" << endl);
            // add a row for this point
            this->MultiColumnList->GetWidget()->AddRow();
            
            // now populate it
            xyz = activeFiducialListNode->GetNthFiducialXYZ(row);
            wxyz = activeFiducialListNode->GetNthFiducialOrientation(row);
            
            if (activeFiducialListNode->GetNthFiducialLabelText(row) != NULL)
            {
                this->MultiColumnList->GetWidget()->SetCellText(row,0,activeFiducialListNode->GetNthFiducialLabelText(row));
            }
            else
            {
                this->MultiColumnList->GetWidget()->SetCellText(row,0,"(none)");
            }
            if (xyz != NULL)
            {
                this->MultiColumnList->GetWidget()->SetCellTextAsDouble(row,1,xyz[0]);
                this->MultiColumnList->GetWidget()->SetCellTextAsDouble(row,2,xyz[1]);
                this->MultiColumnList->GetWidget()->SetCellTextAsDouble(row,3,xyz[2]);
            }
            if (wxyz != NULL)
            {
                this->MultiColumnList->GetWidget()->SetCellTextAsDouble(row,4,wxyz[0]);
                this->MultiColumnList->GetWidget()->SetCellTextAsDouble(row,5,wxyz[1]);
                this->MultiColumnList->GetWidget()->SetCellTextAsDouble(row,6,wxyz[2]);
                this->MultiColumnList->GetWidget()->SetCellTextAsDouble(row,7,wxyz[3]);
            }
            // selected
            this->MultiColumnList->GetWidget()->SetCellTextAsInt(row,8,(activeFiducialListNode->GetNthFiducialSelected(row) ? 1 : 0));
            this->MultiColumnList->GetWidget()->SetCellWindowCommandToCheckButton(row,8);
        }
        
        vtkDebugMacro("Now going to update GUI from the logic's active list");
    // update the visibility, color, scale buttons to match the displayed list's
    if (activeFiducialListNode == NULL)
    {
        std::cerr << "vtkSlicerFiducialsGUI::ProcessMRMLEvents: ERROR: no active fiducial list node in the gui class!\n";                return;
    }
    vtkDebugMacro(<< "\tupdating the visibility button\n");
    if (this->GetVisibilityToggle() != NULL &&
        this->GetVisibilityIcons() != NULL)
    {
        if (activeFiducialListNode->GetVisibility() > 0)
        {
            this->GetVisibilityToggle()->SetImageToIcon(
                this->GetVisibilityIcons()->GetVisibleIcon());
        }
        else
        {
            this->GetVisibilityToggle()->SetImageToIcon(
                this->GetVisibilityIcons()->GetInvisibleIcon());
        }
    }
    else
    {
        std::cerr << "ERROR; trying up update null visibility toggle!\n";
    }
    
    // color
    vtkDebugMacro(<< "\tupdating the colour\n");
    double *nodeColor = activeFiducialListNode->GetColor();
    if (this->ListColorButton != NULL)
    {
        double *buttonColor = this->ListColorButton->GetColor();
        if (nodeColor != NULL && buttonColor != NULL && 
            (nodeColor[0] != buttonColor[0] ||
             nodeColor[1] != buttonColor[1] ||
             nodeColor[2] != buttonColor[2]))
        {
            vtkDebugMacro("Updating list color button\n");
            this->ListColorButton->SetColor(nodeColor);
        }
    }
    else
    {
        std::cerr << "No colour button!\n";
    }

    // selected color
    vtkDebugMacro(<< "\tupdating the selected colour\n");
    double *nodeSelectedColor = activeFiducialListNode->GetSelectedColor();
    if (this->ListSelectedColorButton != NULL)
    {
        double *buttonSelectedColor = this->ListSelectedColorButton->GetColor();
        if (nodeSelectedColor != NULL && buttonSelectedColor != NULL && 
            (nodeSelectedColor[0] != buttonSelectedColor[0] ||
             nodeSelectedColor[1] != buttonSelectedColor[1] ||
             nodeSelectedColor[2] != buttonSelectedColor[2]))
        {
            vtkDebugMacro("Updating list selected color button\n");
            this->ListSelectedColorButton->SetColor(nodeSelectedColor);
        }
    }
    else
    {
        std::cerr << "No selected colour button!\n";
    }
    
    // symbol scale
    vtkDebugMacro(<< "\tupdating the symbol scale");
    double scale = activeFiducialListNode->GetSymbolScale();
    if (this->ListSymbolScale != NULL &&
        scale != this->ListSymbolScale->GetValue())
    {
        this->ListSymbolScale->SetValue(scale);
    }
    

    // text scale
    vtkDebugMacro(<< "\tupdating the text scale.");
    scale = activeFiducialListNode->GetTextScale();
    if (this->ListTextScale != NULL &&
        scale != this->ListTextScale->GetValue())
    {
        this->ListTextScale->SetValue(scale);
    }

    // opacity
    vtkDebugMacro(<< "\tupdating the opacity");
    scale = activeFiducialListNode->GetOpacity();
    if (this->ListOpacity != NULL &&
        scale != this->ListOpacity->GetValue())
    {
        this->ListOpacity->SetValue(scale);
    }
}
//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::Enter ( )
{
    std::cout << "vtkSlicerFiducialsGUI: Enter\n";
    // Fill in - try to go back to the last fiducial
}

//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::Exit ( )
{
    std::cout <<  "vtkSlicerFiducialsGUI: Exit\n";
    // Fill in
    // save the last fiducial we were looking at
    // this->ViewingRow = this->MultiColumnList->?
}




//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::BuildGUI ( )
{
    vtkSlicerApplication *app = (vtkSlicerApplication *)this->GetApplication();

    // ---
    // MODULE GUI FRAME 
    // configure a page for a model loading UI for now.
    // later, switch on the modulesButton in the SlicerControlGUI
    // ---
    // create a page
    this->UIPanel->AddPage ( "Fiducials", "Fiducials", NULL );

    vtkKWWidget *page = this->UIPanel->GetPageWidget ( "Fiducials" );
    
    // HELP FRAME
    vtkKWFrameWithLabel *modHelpFrame = vtkKWFrameWithLabel::New ( );
    modHelpFrame->SetParent (page );
    modHelpFrame->Create ( );
    modHelpFrame->CollapseFrame ( );
    modHelpFrame->SetLabelText ("Help");
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                  modHelpFrame->GetWidgetName(), this->UIPanel->GetPageWidget("Fiducials")->GetWidgetName());

    vtkKWMessage *helpMessage = vtkKWMessage::New();
    helpMessage->SetParent(modHelpFrame->GetFrame());
    helpMessage->Create();
    helpMessage->SetWidth(100);
    helpMessage->SetText("Create and manage Fiducial lists.");
    app->Script ( "pack %s", helpMessage->GetWidgetName());
                  
    // ---
    // DISPLAY FRAME            
    vtkKWFrameWithLabel *displayFrame = vtkKWFrameWithLabel::New ( );
    displayFrame->SetParent ( page );
    displayFrame->Create ( );
    displayFrame->SetLabelText ("Display");
    displayFrame->ExpandFrame ( );
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                  displayFrame->GetWidgetName(),
                  this->UIPanel->GetPageWidget("Fiducials")->GetWidgetName());
  
    // node selector
    this->FiducialListSelectorWidget = vtkSlicerNodeSelectorWidget::New() ;
    this->FiducialListSelectorWidget->SetParent(displayFrame->GetFrame());
    this->FiducialListSelectorWidget->Create();
    this->FiducialListSelectorWidget->SetNodeClass("vtkMRMLFiducialListNode", NULL, NULL, NULL);
    this->FiducialListSelectorWidget->NewNodeEnabledOn();
    this->FiducialListSelectorWidget->SetMRMLScene(this->GetMRMLScene());
    this->FiducialListSelectorWidget->SetBorderWidth(2);
    this->FiducialListSelectorWidget->SetPadX(2);
    this->FiducialListSelectorWidget->SetPadY(2);
    //this->FiducialListSelectorWidget->GetWidget()->IndicatorVisibilityOff();
    this->FiducialListSelectorWidget->GetWidget()->SetWidth(24);
    this->FiducialListSelectorWidget->SetLabelText( "Fiducial List Select: ");
    this->FiducialListSelectorWidget->SetBalloonHelpString("Select a fiducial list from the current mrml scene.");
    this->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2",
                  this->FiducialListSelectorWidget->GetWidgetName());
    
    
    // visibility
    this->VisibilityIcons = vtkSlicerVisibilityIcons::New ( );
    this->VisibilityToggle = vtkKWPushButton::New();
    this->VisibilityToggle->SetParent ( displayFrame->GetFrame() );
    this->VisibilityToggle->Create ( );
    this->VisibilityToggle->SetReliefToFlat ( );
    this->VisibilityToggle->SetOverReliefToNone ( );
    this->VisibilityToggle->SetBorderWidth ( 0 );
    this->VisibilityToggle->SetImageToIcon ( this->VisibilityIcons->GetInvisibleIcon ( ) );        
    this->VisibilityToggle->SetBalloonHelpString ( "Toggles fiducial list visibility in the MainViewer." );
    this->VisibilityToggle->SetText ("Visibility");
    
    //app->Script("pack %s -side left -anchor w -padx 2 -pady 4", 
    //            this->VisibilityToggle->GetWidgetName());

    // opacity
    this->ListOpacity = vtkKWScaleWithEntry::New();
    this->ListOpacity->SetParent( displayFrame->GetFrame() );
    this->ListOpacity->Create();
    this->ListOpacity->SetLabelText("Opacity:");
    this->ListOpacity->SetBalloonHelpString ( "Set the opacity of the fiducial list symbols.");
    this->ListOpacity->GetWidget()->SetRange(0.0, 1.0);
    this->ListOpacity->GetWidget()->SetOrientationToHorizontal();
    this->ListOpacity->GetWidget()->SetResolution(0.1);
    this->ListOpacity->SetEntryWidth(5);
    app->Script("pack %s %s -side top -anchor w -padx 2 -pady 2", 
                this->VisibilityToggle->GetWidgetName(), this->ListOpacity->GetWidgetName());

        // text scale
    this->ListTextScale = vtkKWScaleWithEntry::New();
    this->ListTextScale->SetParent( displayFrame->GetFrame() );
    this->ListTextScale->Create();
    this->ListTextScale->SetLabelText("Text Scale:");
    this->ListTextScale->SetBalloonHelpString ( "Set the scale of the fiducial list points text.");
    this->ListTextScale->GetWidget()->SetRange(0.0, 20.0);
    this->ListTextScale->GetWidget()->SetOrientationToHorizontal ();
    this->ListTextScale->GetWidget()->SetResolution(0.5);
    this->ListTextScale->SetEntryWidth(5);
    app->Script("pack %s -side top -anchor w -padx 2 -pady 2", 
                this->ListTextScale->GetWidgetName());

    // symbol scale
    this->ListSymbolScale = vtkKWScaleWithEntry::New();
    this->ListSymbolScale->SetParent( displayFrame->GetFrame() );
    this->ListSymbolScale->Create();
    this->ListSymbolScale->SetLabelText("Symbol Scale:");
    this->ListSymbolScale->SetBalloonHelpString ( "Set the scale of the fiducial list symbols.");
    this->ListSymbolScale->GetWidget()->SetRange(0.0, 80.0);
    this->ListSymbolScale->GetWidget()->SetOrientationToHorizontal();
    this->ListSymbolScale->GetWidget()->SetResolution(0.5);
    this->ListSymbolScale->SetEntryWidth(5);
    app->Script("pack %s -side top -anchor w -padx 2 -pady 2", 
                this->ListSymbolScale->GetWidgetName());

    
    // color
    this->ListColorButton = vtkKWChangeColorButton::New();
    this->ListColorButton->SetParent( displayFrame->GetFrame() );
    this->ListColorButton->Create();
    this->ListColorButton->SetBorderWidth(0);
    this->ListColorButton->SetBalloonHelpString("Change the colour of the fiducial list symbols and text in the MainViewer");
    this->ListColorButton->SetDialogTitle("List symbol and text color");
    this->ListColorButton->SetLabelText("Set Color");

    // selected colour
    this->ListSelectedColorButton = vtkKWChangeColorButton::New();
    this->ListSelectedColorButton->SetParent( displayFrame->GetFrame() );
    this->ListSelectedColorButton->Create();
    this->ListSelectedColorButton->SetBorderWidth(0);
    this->ListSelectedColorButton->SetBalloonHelpString("Change the colour of the selected fiducial list symbols and text in the MainViewer");
    
    this->ListSelectedColorButton->SetDialogTitle("List selected symbol and text color");
    this->ListSelectedColorButton->SetLabelText("Set Selected Color");

    // pack the colours 
    app->Script("pack %s %s -side top -anchor w -padx 2 -pady 2",
                this->ListColorButton->GetWidgetName(), this->ListSelectedColorButton->GetWidgetName());
    
    // ---
    // LIST FRAME
    
    vtkKWFrameWithLabel *listFrame = vtkKWFrameWithLabel::New();
    listFrame->SetParent( page );
    listFrame->Create();
    listFrame->SetLabelText("Fiducial List");
    listFrame->ExpandFrame();
    app->Script ( "pack %s -side top -anchor nw -fill x -padx 2 -pady 2 -in %s",
                  listFrame->GetWidgetName(), this->UIPanel->GetPageWidget("Fiducials")->GetWidgetName());

    
    

    
    // add the multicolumn list to show the points
    this->MultiColumnList = vtkKWMultiColumnListWithScrollbars::New ( );
    this->MultiColumnList->SetParent ( listFrame->GetFrame() );
    this->MultiColumnList->Create ( );
    this->MultiColumnList->SetHeight(4);
    this->MultiColumnList->GetWidget()->SetSelectionTypeToCell();
    this->MultiColumnList->GetWidget()->MovableRowsOff();
    this->MultiColumnList->GetWidget()->MovableColumnsOff();
    // set up the columns of data for each point
    // name, x, y, z, orientation w, x, y, z, selected
    this->MultiColumnList->GetWidget()->AddColumn("Name");
    this->MultiColumnList->GetWidget()->AddColumn("X");
    this->MultiColumnList->GetWidget()->AddColumn("Y");
    this->MultiColumnList->GetWidget()->AddColumn("Z");
    this->MultiColumnList->GetWidget()->AddColumn("OrW");
    this->MultiColumnList->GetWidget()->AddColumn("OrX");
    this->MultiColumnList->GetWidget()->AddColumn("OrY");
    this->MultiColumnList->GetWidget()->AddColumn("OrZ");
    this->MultiColumnList->GetWidget()->AddColumn("Selected");

    // make the selected column editable by checkbox
    this->MultiColumnList->GetWidget()->SetColumnEditWindowToCheckButton(8);
    
    
    // now set the attributes that are equal across the columns
    int col;
    for (col = 0; col < this->NumberOfColumns; col++)
    {        
        this->MultiColumnList->GetWidget()->SetColumnWidth(col, 12);
        this->MultiColumnList->GetWidget()->SetColumnAlignmentToLeft(col);
        this->MultiColumnList->GetWidget()->ColumnEditableOn(col);
    }
    app->Script ( "pack %s -fill both -expand true",
                  this->MultiColumnList->GetWidgetName());
//                  listFrame->GetWidgetName());
    this->MultiColumnList->GetWidget()->SetCellUpdatedCommand(this, "UpdateElement");

     // add an add fiducial button
    this->AddFiducialButton = vtkKWPushButton::New ( );
    this->AddFiducialButton->SetParent ( listFrame->GetFrame() );
    this->AddFiducialButton->Create ( );
    this->AddFiducialButton->SetText ("Add Fiducial Point");
    
    app->Script("pack %s -side top -anchor w -padx 2 -pady 4", 
                this->AddFiducialButton->GetWidgetName());

    if (this->MRMLScene != NULL)
    {
        if (this->MRMLScene->HasObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand ) == 0)
        {
            this->MRMLScene->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand );
        }
    }

     
    helpMessage->Delete ( );
    displayFrame->Delete ( );
    listFrame->Delete();
    modHelpFrame->Delete ( );
}

//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::UpdateElement(int row, int col, char * str)
{
    vtkDebugMacro("row = " << row << ", col = " << col << ", str = " << str << "\n");

    // make sure that the row and column exists in the table
    if ((row >= 0) && (row < this->MultiColumnList->GetWidget()->GetNumberOfRows()) &&
        (col >= 0) && (col < this->MultiColumnList->GetWidget()->GetNumberOfColumns()))
        {
            vtkMRMLFiducialListNode *activeFiducialListNode = (vtkMRMLFiducialListNode *)this->MRMLScene->GetNodeByID(this->GetFiducialListNodeID());
            // is there an active list?
            if (activeFiducialListNode == NULL)
            {
                // 
                std::cerr << "UpdateElement: ERROR: No Fiducial List, add one first!\n";
                return;
            }
        
            // now update the requested value
            if (col == 0)
            {
                activeFiducialListNode->SetNthFiducialLabelText(row, str);
            }
            else if (col > 0 && col < 4)
            {
                // get the current xyz
                float * xyz = activeFiducialListNode->GetNthFiducialXYZ(row);
                // now set the new one
                float newCoordinate = atof(str);
                if (col == 1) { activeFiducialListNode->SetNthFiducialXYZ(row, newCoordinate, xyz[1], xyz[2]); }
                if (col == 2) { activeFiducialListNode->SetNthFiducialXYZ(row, xyz[0], newCoordinate, xyz[2]); }
                if (col == 3) { activeFiducialListNode->SetNthFiducialXYZ(row, xyz[0], xyz[1], newCoordinate); }
            }
            else if (col > 3 && col < 8)
            {
                float * wxyz = activeFiducialListNode->GetNthFiducialOrientation(row);
                float newCoordinate = atof(str);
                if (col == 4) { activeFiducialListNode->SetNthFiducialOrientation(row, newCoordinate, wxyz[1], wxyz[2], wxyz[3]); }
                if (col == 5) { activeFiducialListNode->SetNthFiducialOrientation(row, wxyz[0], newCoordinate, wxyz[2], wxyz[3]); }
                if (col == 6) { activeFiducialListNode->SetNthFiducialOrientation(row, wxyz[0], wxyz[1], newCoordinate, wxyz[3]); }
                if (col == 7) { activeFiducialListNode->SetNthFiducialOrientation(row, wxyz[0], wxyz[1], wxyz[2], newCoordinate); }
            }
            else if (col == 8)
            {
                // selected
                vtkDebugMacro("UpdateElement: setting node " <<  activeFiducialListNode->GetNthFiducialLabelText(row) << "'s selected flag to " << str << endl);
                activeFiducialListNode->SetNthFiducialSelected(row, atoi(str));
            }
            else
            {
                std::cerr << "UpdateElement: ERROR: invalid column number " << col << ", valid values are 0-" << this->NumberOfColumns << endl;
                return;
            }
        }
    else
    {
        std::cerr << "Invalid row " << row << " or column " << col <<  ", valid columns are 0-" << this->NumberOfColumns << "\n";
    }
}

//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::SetFiducialListNode (vtkMRMLFiducialListNode *fiducialListNode)
{
    if (fiducialListNode == NULL)
    {
        std::cerr << "ERROR: SetFiducialListNode - list node is null.\n";
        return;
    }
    // save the ID
    vtkDebugMacro("setting the fid list node id to " << fiducialListNode->GetID());
    this->SetFiducialListNodeID(fiducialListNode->GetID());
}

//---------------------------------------------------------------------------
void vtkSlicerFiducialsGUI::SetFiducialListNodeID (char * id)
{    
    if (id == NULL)
    {
        std::cerr << "SetFiducialListNodeID: NULL input id, returning.\n";
        return;
    }
    if (this->GetFiducialListNodeID() != NULL &&
        strcmp(id,this->GetFiducialListNodeID()) == 0)
    {
        vtkDebugMacro("no change in id, not doing anything for now: " << id << endl);
        return;
    }

    // get the old node
    vtkMRMLFiducialListNode *fidlist = vtkMRMLFiducialListNode::SafeDownCast(this->MRMLScene->GetNodeByID(this->GetFiducialListNodeID()));
    if (fidlist != NULL)
    {
        // remove observers on old node
        fidlist->RemoveObservers(vtkMRMLFiducialListNode::DisplayModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand);
        fidlist->RemoveObservers(vtkCommand::ModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand);
        fidlist->RemoveObservers(vtkMRMLFiducialListNode::FiducialModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand);
    }
    else
    {
        // probably wasn't set yet
    }
    // set the id properly - see the vtkSetStringMacro
    this->FiducialListNodeID = id;

    // get the new node
    fidlist = vtkMRMLFiducialListNode::SafeDownCast(this->MRMLScene->GetNodeByID(this->GetFiducialListNodeID()));
    // set up observers on the new node
    if (fidlist != NULL)
    {
        if (fidlist->HasObserver(vtkMRMLFiducialListNode::DisplayModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand) == 0)
        {
            fidlist->AddObserver(vtkMRMLFiducialListNode::DisplayModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand);
        }
        if (fidlist->HasObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand) == 0)
        {
            fidlist->AddObserver(vtkCommand::ModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand);
        }
        if (fidlist->HasObserver(vtkMRMLFiducialListNode::FiducialModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand) == 0)
        {
            fidlist->AddObserver(vtkMRMLFiducialListNode::FiducialModifiedEvent, (vtkCommand *)this->MRMLCallbackCommand);
        }
        // set up the GUI
        this->SetGUIFromList(fidlist);
    }
    else
    {
        std::cerr << "ERROR: unable to get the mrml fiducial node to observe!\n";
    }
}
