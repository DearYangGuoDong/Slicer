/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

// MRMLCLI includes
#include "vtkMRMLCommandLineModuleNode.h"

// MRML includes

/// SlicerExecutionModel includes
#include <ModuleDescription.h>

// VTK includes
#include "vtkObjectFactory.h"

// STD includes
#include <sstream>


//------------------------------------------------------------------------------
// Private implementaton of an std::map
class ModuleDescriptionMap : public std::map<std::string, ModuleDescription> {};

//------------------------------------------------------------------------------
class vtkMRMLCommandLineModuleNode::vtkInternal
{
public:

  ModuleDescription ModuleDescriptionObject;

  static ModuleDescriptionMap RegisteredModules;

  vtkMRMLCommandLineModuleNode::StatusType Status;
};

ModuleDescriptionMap vtkMRMLCommandLineModuleNode::vtkInternal::RegisteredModules;

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkMRMLCommandLineModuleNode);

//----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLCommandLineModuleNode::CreateNodeInstance()
{
  return vtkMRMLCommandLineModuleNode::New();
}

//----------------------------------------------------------------------------
vtkMRMLCommandLineModuleNode::vtkMRMLCommandLineModuleNode()
{
  this->Internal = new vtkInternal();
  this->HideFromEditors = true;
  this->Internal->Status = Idle;
}

//----------------------------------------------------------------------------
vtkMRMLCommandLineModuleNode::~vtkMRMLCommandLineModuleNode()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::WriteXML(ostream& of, int nIndent)
{
  // Serialize a CommandLineModule node.
  //
  // Only need to write out enough information from the
  // ModuleDescription such that we can recognize the node type.  When
  // we reconstitute a node, we will start with a copy of the
  // prototype node for that module and then overwrite individual
  // parameter values using the parameter values indicated here.


  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  const ModuleDescription& module = this->GetModuleDescription();

  // Need to write out module description and parameters as
  // attributes.  Only need to write out the module title and version
  // in order to be able recognize the node type.  Then we just need
  // to write out each parameter name and default.  Note that any
  // references to other nodes are already stored as IDs. So we write
  // out those IDs.
  //
  of << " title=\"" << this->URLEncodeString ( module.GetTitle().c_str() ) << "\"";
  of << " version=\"" << this->URLEncodeString ( module.GetVersion().c_str() ) << "\"";
  
  // Loop over the parameter groups, writing each parameter.  Note
  // that the parameter names are unique.
  std::vector<ModuleParameterGroup>::const_iterator pgbeginit
    = module.GetParameterGroups().begin();
  std::vector<ModuleParameterGroup>::const_iterator pgendit
    = module.GetParameterGroups().end();
  std::vector<ModuleParameterGroup>::const_iterator pgit;

  
  for (pgit = pgbeginit; pgit != pgendit; ++pgit)
    {
    // iterate over each parameter in this group
    std::vector<ModuleParameter>::const_iterator pbeginit
      = (*pgit).GetParameters().begin();
    std::vector<ModuleParameter>::const_iterator pendit
      = (*pgit).GetParameters().end();
    std::vector<ModuleParameter>::const_iterator pit;

    for (pit = pbeginit; pit != pendit; ++pit)
      {
      // two calls, as the mrml node method saves the new string in a member
      // variable and it was getting over written when used twice before the
      // buffer was flushed.
      of << " " << this->URLEncodeString ( (*pit).GetName().c_str() );
      of  << "=\"" << this->URLEncodeString ( (*pit).GetDefault().c_str() ) << "\"";
      }
    }
  
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  // To reconstitute a CommandLineModule node:
  //
  // 1. Find the prototype node from the "title" and "version".
  // 2. Copy the prototype node into the current node.
  // 3. Override parameter values with the attributes (attributes not
  // consumed by the superclass or known attributes from the prototype
  // node).
  //
  // Referenced nodes are stored as IDs.  Do we need to remap them at all?

  // first look for the title which we need to find the prototype node
  std::string moduleTitle;
  std::string moduleVersion;

  const char **tatts = atts;
  const char *attName = NULL;
  const char *attValue;
  while (*tatts)
    {
    attName = *(tatts++);
    attValue = *(tatts++);

    if (!strcmp(attName, "title"))
      {
      moduleTitle = this->URLDecodeString(attValue);
      }
    else if (!strcmp(attName, "version"))
      {
      moduleVersion = this->URLDecodeString(attValue);
      }
    }

  // Set an attribute on the node based on the module title so that
  // the node selectors can filter on it.
  this->SetAttribute("CommandLineModule", moduleTitle.c_str());
  
  // look up the module description from the library
  if (vtkMRMLCommandLineModuleNode::HasRegisteredModule( moduleTitle ))
    {
    this->Internal->ModuleDescriptionObject =
     vtkMRMLCommandLineModuleNode::GetRegisteredModuleDescription(moduleTitle);
    }
  else
    {
    // can't locate the module, return;
    return;
    }

  // Verify the version
  if (moduleVersion != this->Internal->ModuleDescriptionObject.GetVersion())
    {
    std::string msg = "Command line module " + moduleTitle + " is version \""
      + this->Internal->ModuleDescriptionObject.GetVersion()
      + "\" but parameter set from MRML file is version \""
      + moduleVersion
      + "\". Parameter set may not load properly,";
      
    vtkWarningMacro(<< msg.c_str());
    }
  
  // run through the attributes and pull out any attributes for this
  // module
  tatts = atts;
  while (*tatts)
    {
    std::string sattName = std::string(this->URLDecodeString(*(tatts++)));
    std::string sattValue = std::string(this->URLDecodeString(*(tatts++)));

    if (this->Internal->ModuleDescriptionObject.HasParameter(attName))
      {
      this->Internal->ModuleDescriptionObject.SetParameterDefaultValue(sattName.c_str(),sattValue.c_str());
      }
    }
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLCommandLineModuleNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLCommandLineModuleNode *node = (vtkMRMLCommandLineModuleNode *) anode;

  this->SetModuleDescription(node->GetModuleDescription());
  this->SetStatus(node->GetStatus());
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "Module description:   "
     << std::endl
     << "   " << this->GetModuleDescription();
  os << indent << "Status: " << this->GetStatus();
}

//----------------------------------------------------------------------------
const ModuleDescription& vtkMRMLCommandLineModuleNode::GetModuleDescription() const
{
  return this->Internal->ModuleDescriptionObject;
}

//----------------------------------------------------------------------------
ModuleDescription& vtkMRMLCommandLineModuleNode::GetModuleDescription()
{
  return this->Internal->ModuleDescriptionObject;
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::SetModuleDescription(const ModuleDescription& description)
{
  // Copy the module description
  this->Internal->ModuleDescriptionObject = description;

  // Set an attribute on the node so that we can select nodes that
  // have the same command line module (program)
  this->SetAttribute("CommandLineModule", description.GetTitle().c_str());

  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode
::SetParameterAsString(const std::string& name, const std::string& value)
{
  // Set the default value of the named parameter with the value
  // specified
  if (value != this->GetParameterAsString(name))
    {
    this->Internal->ModuleDescriptionObject.SetParameterDefaultValue(name, value);
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode
::SetParameterAsDouble(const std::string& name, double value)
{
  std::ostringstream strvalue;

  strvalue << value;
  
  // Set the default value of the named parameter with the value
  // specified
  if (strvalue.str() != this->GetParameterAsString(name))
    {
    this->Internal->ModuleDescriptionObject
      .SetParameterDefaultValue(name, strvalue.str());
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode
::SetParameterAsFloat(const std::string& name, float value)
{
  std::ostringstream strvalue;

  strvalue << value;
  
  // Set the default value of the named parameter with the value
  // specified
  if (strvalue.str() != this->GetParameterAsString(name))
    {
    this->Internal->ModuleDescriptionObject
      .SetParameterDefaultValue(name, strvalue.str());
    this->Modified();
    }
}


//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode
::SetParameterAsInt(const std::string& name, int value)
{
  std::ostringstream strvalue;

  strvalue << value;
  
  // Set the default value of the named parameter with the value
  // specified
  if (strvalue.str() != this->GetParameterAsString(name))
    {
    this->Internal->ModuleDescriptionObject
      .SetParameterDefaultValue(name, strvalue.str());
    this->Modified();
    }
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode
::SetParameterAsBool(const std::string& name, bool value)
{
  // Set the default value of the named parameter with the value
  // specified
  if (this->GetParameterAsString(name) != (value ? "true" : "false"))
    {
    this->Internal->ModuleDescriptionObject
      .SetParameterDefaultValue(name, value ? "true" : "false");
    this->Modified();
    }
}

//----------------------------------------------------------------------------
std::string vtkMRMLCommandLineModuleNode::GetParameterAsString(const std::string& name) const
{
  return this->Internal->ModuleDescriptionObject.GetParameterDefaultValue(name);
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::SetParameterAsString(const char *name, const char *value)
{
  this->SetParameterAsString(std::string(name), std::string(value));
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::SetParameterAsInt(const char *name, const int value)
{
  this->SetParameterAsInt(std::string(name), value);
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::SetParameterAsBool(const char *name, const int value)
{
  this->SetParameterAsBool(std::string(name), (value == 0 ? false : true));
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::SetParameterAsDouble(const char *name, const double value)
{
  this->SetParameterAsDouble(std::string(name), value);
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::SetParameterAsFloat(const char *name, const float value)
{
  this->SetParameterAsFloat(std::string(name), value);
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterAsString(const char* name) const
{
  std::string stringName = name;
  return this->GetParameterAsString(stringName).c_str();
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode
::SetStatus(vtkMRMLCommandLineModuleNode::StatusType status, bool modify)
{
  if (this->Internal->Status != status)
    {
    this->Internal->Status = status;
    if (this->Internal->Status == vtkMRMLCommandLineModuleNode::Cancelled)
      {
      this->AbortProcess();
      }
    if (modify)
      {
      this->Modified();
      }
    }
}

//----------------------------------------------------------------------------
vtkMRMLCommandLineModuleNode::StatusType vtkMRMLCommandLineModuleNode::GetStatus()
{
  return this->Internal->Status;
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetStatusString()
{
  switch (this->Internal->Status)
    {
    case Idle: return "Idle";
    case Scheduled: return "Scheduled";
    case Running: return "Running";
    case Completed: return "Completed";
    case CompletedWithErrors: return "CompletedWithErrors";
    case Cancelled: return "Cancelled";
    }
  return "Unknown";
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::AbortProcess()
{
  this->GetModuleDescription().GetProcessInformation()->Abort = 1;
}

//----------------------------------------------------------------------------
bool vtkMRMLCommandLineModuleNode::HasRegisteredModule(const std::string& name)
{
  ModuleDescriptionMap::iterator mit =
      vtkMRMLCommandLineModuleNode::vtkInternal::RegisteredModules.find(name);

  return mit != vtkMRMLCommandLineModuleNode::vtkInternal::RegisteredModules.end();
}

//----------------------------------------------------------------------------
int vtkMRMLCommandLineModuleNode::GetNumberOfRegisteredModules()
{
  return (int)vtkInternal::RegisteredModules.size();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetRegisteredModuleNameByIndex( int idx )
{
  ModuleDescriptionMap::iterator mit = vtkInternal::RegisteredModules.begin();
  int count = 0;
  while ( mit != vtkInternal::RegisteredModules.end() )
    {
    if ( count == idx ) { return (*mit).first.c_str(); }
    ++mit;
    ++count;
    }
  return "";
}

//----------------------------------------------------------------------------
ModuleDescription vtkMRMLCommandLineModuleNode
::GetRegisteredModuleDescription(const std::string& name)
{
  ModuleDescriptionMap::iterator mit =
      vtkMRMLCommandLineModuleNode::vtkInternal::RegisteredModules.find(name);

  if (mit != vtkMRMLCommandLineModuleNode::vtkInternal::RegisteredModules.end())
    {
    return (*mit).second;
    }

  return ModuleDescription();
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode
::RegisterModuleDescription(ModuleDescription md)
{
  vtkMRMLCommandLineModuleNode::vtkInternal::RegisteredModules[md.GetTitle()] = md;
}

//----------------------------------------------------------------------------
void vtkMRMLCommandLineModuleNode::SetModuleDescription ( const char *name )
{
  this->SetModuleDescription ( this->GetRegisteredModuleDescription ( name ) );
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetModuleVersion ()
{
  return this->GetModuleDescription().GetVersion().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetModuleTitle ()
{
  return this->GetModuleDescription().GetTitle().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetModuleTarget ()
{
  return this->GetModuleDescription().GetTarget().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetModuleType ()
{
  return this->GetModuleDescription().GetType().c_str();
}

//----------------------------------------------------------------------------
unsigned int vtkMRMLCommandLineModuleNode::GetNumberOfParameterGroups ()
{
  return (int)this->GetModuleDescription().GetParameterGroups().size();
}

//----------------------------------------------------------------------------
unsigned int vtkMRMLCommandLineModuleNode::GetNumberOfParametersInGroup ( unsigned int group )
{
  if ( group >= this->GetModuleDescription().GetParameterGroups().size() ) { return 0; }
  return (unsigned int)this->GetModuleDescription().GetParameterGroups()[group].GetParameters().size();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterGroupLabel ( unsigned int group )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetLabel().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterGroupDescription ( unsigned int group )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetDescription().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterGroupAdvanced ( unsigned int group )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetAdvanced().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterTag ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetTag().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterType ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetType().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterArgType ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetArgType().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterName ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetName().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterLongFlag ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetLongFlag().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterLabel ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetLabel().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterConstraints ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetConstraints().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterMaximum ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetMaximum().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterMinimum ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetMinimum().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterDescription ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetDescription().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterChannel ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetChannel().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterIndex ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetIndex().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterDefault ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetDefault().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterFlag ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetFlag().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterMultiple ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetMultiple().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterFileExtensions ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetFileExtensionsAsString().c_str();
}

//----------------------------------------------------------------------------
const char* vtkMRMLCommandLineModuleNode::GetParameterCoordinateSystem ( unsigned int group, unsigned int param )
{
  return this->GetModuleDescription().GetParameterGroups()[group].GetParameters()[param].GetCoordinateSystem().c_str();
}

//----------------------------------------------------------------------------
bool vtkMRMLCommandLineModuleNode::ReadParameterFile(const std::string& filename)
{
  return this->ReadParameterFile(filename.c_str());
}

//----------------------------------------------------------------------------
bool vtkMRMLCommandLineModuleNode::ReadParameterFile(const char* filename)
{
  bool modified = this->Internal->ModuleDescriptionObject.ReadParameterFile(filename);
  
  if (modified)
    {
    this->Modified();
    }

  return modified;
}

//----------------------------------------------------------------------------
bool vtkMRMLCommandLineModuleNode
::WriteParameterFile(const std::string& filename, bool withHandlesToBulkParameters)
{
  return this->WriteParameterFile(filename.c_str(), withHandlesToBulkParameters);
}

//----------------------------------------------------------------------------
bool vtkMRMLCommandLineModuleNode
::WriteParameterFile(const char* filename, bool withHandlesToBulkParameters)
{
  bool modified 
    = this->Internal->ModuleDescriptionObject.WriteParameterFile(filename, withHandlesToBulkParameters);
  
  if (modified)
    {
    this->Modified();
    }

  return modified;
}
