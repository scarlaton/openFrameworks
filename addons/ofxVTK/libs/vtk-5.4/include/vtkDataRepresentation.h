/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDataRepresentation.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/
// .NAME vtkDataRepresentation - The superclass for all representations
//
// .SECTION Description
// vtkDataRepresentation the superclass for representations of data objects.
// This class itself may be instantiated and used as a representation
// that simply holds a connection to a pipeline.
//
// If there are multiple representations present in a view, you should use
// a subclass of vtkDataRepresentation.  The representation is
// responsible for taking the input pipeline connection and converting it
// to an object usable by a view.  In the most common case, the representation
// will contain the pipeline necessary to convert a data object into an actor
// or set of actors.
//
// The representation has a concept of a selection.  If the user performs a
// selection operation on the view, the view forwards this on to its
// representations.  The representation is responsible for displaying that
// selection in an appropriate way.
//
// Representation selections may also be linked.  The representation shares
// the selection by converting it into a view-independent format, then
// setting the selection on its vtkSelectionLink.  Other representations
// sharing the same selection link instance will get the same selection
// from the selection link when the view is updated.  The application is
// responsible for linking representations as appropriate by setting the
// same vtkSelectionLink on each linked representation.

#ifndef __vtkDataRepresentation_h
#define __vtkDataRepresentation_h

#include "vtkObject.h"

class vtkAlgorithmOutput;
class vtkConvertSelectionDomain;
class vtkDataObject;
class vtkSelection;
class vtkSelectionLink;
class vtkTable;
class vtkView;

class VTK_VIEWS_EXPORT vtkDataRepresentation : public vtkObject
{
public:
  static vtkDataRepresentation *New();
  vtkTypeRevisionMacro(vtkDataRepresentation, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);
  
  // Description:
  // A convenience method that calls SetInputConnection on the producer's
  // connection.
  void SetInput(vtkDataObject* input);
  
  // Description:
  // Sets the input pipeline connection to this representation.
  // This method should be overridden by subclasses to connect to the
  // internal pipeline.
  virtual void SetInputConnection(vtkAlgorithmOutput* conn);
  virtual vtkAlgorithmOutput* GetInputConnection()
    { return this->GetInputConnectionInternal(); }
  
  // Description:
  // Handle a selection from a view.  Subclasses should not generally override this.
  // This function calls ConvertSelection() to (possibly) convert the selection
  // into something appropriate for linked representations.
  // This function then calls UpdateSelection() with the result of the
  // conversion.
  void Select(vtkView* view, vtkSelection* selection);

  // Description:
  // Whether this representation is able to handle a selection.
  // Default is true.
  vtkSetMacro(Selectable, bool);
  vtkGetMacro(Selectable, bool);
  vtkBooleanMacro(Selectable, bool);
  
  // Description:
  // Copies the selection into the representation's linked selection
  // and triggers a SelectionChanged event.
  void UpdateSelection(vtkSelection* selection);
  
  // Description:
  // The selection linking object.
  // Set the same selection link on multiple representations to link views.
  // Subclasses should override SetSelectionLink to additionally connect
  // the selection link into the internal selection pipeline.
  virtual vtkSelectionLink* GetSelectionLink()
    { return this->GetSelectionLinkInternal(); }
  virtual void SetSelectionLink(vtkSelectionLink* link);

  // Description:
  // The output port that contains the selection after it has gone through
  // the domain map. ExtractSelection filters in views/representations
  // should use this port for the selection input.
  vtkAlgorithmOutput* GetSelectionConnection();
  
protected:
  vtkDataRepresentation();
  ~vtkDataRepresentation();
  
  // Decription:
  // Adds the representation to the view.  This is called from
  // vtkView::AddRepresentation().  Subclasses should override this method.
  virtual bool AddToView(vtkView* vtkNotUsed(view)) { return true; }
  
  // Decription:
  // Removes the representation to the view.  This is called from
  // vtkView::RemoveRepresentation().  Subclasses should override this method.
  virtual bool RemoveFromView(vtkView* vtkNotUsed(view)) { return true; }
  
  // Description:
  // Convert the selection to a type appropriate for sharing with other
  // representations through vtkSelectionLink, possibly using the view.
  // For the superclass, we just return the same selection.
  // Subclasses may do something more fancy, like convert the selection
  // from a frustrum to a list of pedigree ids.  If the selection cannot
  // be applied to this representation, return NULL.
  virtual vtkSelection* ConvertSelection(vtkView* view, vtkSelection* selection);
  
  // The input connection.
  vtkGetObjectMacro(InputConnectionInternal, vtkAlgorithmOutput);
  void SetInputConnectionInternal(vtkAlgorithmOutput* conn);
  vtkAlgorithmOutput* InputConnectionInternal;
  
  // The linked selection.
  vtkGetObjectMacro(SelectionLinkInternal, vtkSelectionLink);
  void SetSelectionLinkInternal(vtkSelectionLink* link);
  vtkSelectionLink* SelectionLinkInternal;

  // The filter used to map the selection to the appropriate domain.
  vtkConvertSelectionDomain* ConvertDomain;

  // Whether is represenation can handle a selection.
  bool Selectable;

  //BTX
  friend class vtkView;
  //ETX

private:
  vtkDataRepresentation(const vtkDataRepresentation&);  // Not implemented.
  void operator=(const vtkDataRepresentation&);  // Not implemented.
};

#endif
