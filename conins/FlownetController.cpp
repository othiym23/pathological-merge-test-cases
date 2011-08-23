/*
 * FLOWNET Complex Pipe Network Solver
 */

// FlownetCtlr.cpp : implementation of the CFlownetController class
//

#include "stdafx.h"
#include "FlownetApp.h"
#include "FlownetController.h"
#include "FlownetDoc.h"
#include "FlownetViewport.h"
#include "MainFrame.h"
#include "FlownetComponent.h"
#include "CFNClass.h"
#include "CNode.h"
#include "ElementData.h"
#include "ProjectTreeCtrl.h"
#include "TreeNetwork.h"
#include "CDBController.h"

#include "FnPropertyDefs.h" // WAL 2001/11/11 had to add to get Flownet to compile

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CFlownetController, CODController)
	//{{AFX_MSG_MAP(CFlownetController)
	ON_COMMAND(ID_LINK_NORMAL,     OnCreateNormalLink)
	ON_COMMAND(ID_LINK_ORTHOGONAL, OnCreateOrthogonalLink)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_EDIT_COPY,  OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT,   OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_OD_DRAW_SELECT, OnDrawSelect)
	ON_COMMAND(ID_OD_DRAW_TEXT, OnDrawText)
	ON_COMMAND(ID_TOGGLE_PHANTOMS,OnTogglePhantomElements)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE,OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_LINK_NORMAL,OnUpdateLinkNormal)
	ON_UPDATE_COMMAND_UI(ID_LINK_ORTHOGONAL,OnUpdateLinkOrthogonal)
	ON_UPDATE_COMMAND_UI(ID_TOGGLE_PHANTOMS,OnUpdateTogglePhantomElements)

END_MESSAGE_MAP()

extern UINT WM_INSERT_NODE1;

bool CFlownetController::m_bClipboardSerialize = false;
bool CFlownetController::m_bClipboardCopy = false;
bool CFlownetController::m_bImporting = false;

/////////////////////////////////////////////////////////////////////////////
// CFlownetController

CFlownetController::CFlownetController(CFlownetViewport* pViewport)
	: CODController(pViewport),
	m_wndWorkspace( ((CMainFrame*)AfxGetMainWnd())->m_wndWorkspace ),
	m_pMainFrm( (CMainFrame*)AfxGetMainWnd() ),
	m_bPhantomsVisible(TRUE),
	m_dwResID(0)
{
	m_compSetTracker.SetAnchorPoint(OD_CENTER); // ensure that components snap to the grid at their centers
}

CFlownetController::~CFlownetController()
{
}

CNodeComponent* CFlownetController::CreateNodeComponent(DWORD dwResID)
{
	CNodeComponent* pNode = new CNodeComponent();
	pNode->Create(dwResID);
	m_dwResID = dwResID;	
	pNode->EnableCenterPort(TRUE);
	return pNode;
}

CElementComponent* CFlownetController::CreateElementComponent(DWORD dwResID)
{
	CElementComponent* pElement = new CElementComponent();
	pElement->Create(dwResID);
	m_dwResID = dwResID;	

	CString sElemType = pElement->GetType();
	if (sElemType != _T("SpecialPIDController") || sElemType != _T("SpecialConductiveHeatTransfer") )
	  pElement->EnableCenterPort(TRUE);

	return pElement;
}

void CFlownetController::SetPropertiesNodeComponent(CNodeComponent* pLinkComp)
{
}

void CFlownetController::SetPropertiesElementComponent(CElementComponent* pLinkComp)
{
}

CODLinkComponent* CFlownetController::CreateNormalLink()
{
	CFlownetLink* pLinkComp = new CFlownetLink();
	pLinkComp->Create();
	m_dwResID = IDR_NORMAL_LINK;	
	pLinkComp->SetTargetEndpoint(new CODArrowEndpoint());
	return pLinkComp;
}

CODLinkComponent* CFlownetController::CreateOrthogonalLink()
{
	CFlownetLink* pLinkComp = new CFlownetLink();
	pLinkComp->CreateOrthogonal();
	m_dwResID = IDR_ORTHOGONAL_LINK;	
	pLinkComp->SetTargetEndpoint(new CODArrowEndpoint());
	return pLinkComp;
}

void CFlownetController::SetPropertiesNormalLink(CODLinkComponent* pLinkComp)
{
	pLinkComp->SetType(_T("Normal Link"));
	pLinkComp->SetName(_T("Normal Link"));
}

void CFlownetController::SetPropertiesCHTLink(CODLinkComponent* pLinkComp)
{
	pLinkComp->SetType(_T("CHT Link"));
	pLinkComp->SetName(_T("CHT Link"));

	CODLineComponent* pLine = pLinkComp->GetLine();
	pLine->SetValue(OD_PROP_LINE + OD_LINE_COLOR, RGB(255,128,0));
	pLine->SetValue(OD_PROP_LINE + OD_LINE_WIDTH, 2);
}

void CFlownetController::SetPropertiesPhantomLink(CODLinkComponent* pLinkComp)
{
	pLinkComp->SetType(_T("Phantom Link"));
	pLinkComp->SetName(_T("Phantom Link"));

	CODLineComponent* pLine = pLinkComp->GetLine();
	pLine->SetValue(OD_PROP_LINE + OD_LINE_COLOR, RGB(192,0,192));
	pLine->SetValue(OD_PROP_LINE + OD_LINE_WIDTH, 1);
}

void CFlownetController::SetPropertiesMasterSlaveLink(CODLinkComponent* pLinkComp)
{
	pLinkComp->SetType(_T("MasterSlave Link"));
	pLinkComp->SetName(_T("MasterSlave Link"));

	CODLineComponent* pLine = pLinkComp->GetLine();
	pLine->SetValue(OD_PROP_LINE + OD_LINE_COLOR, RGB(255,128,0));
	pLine->SetValue(OD_PROP_LINE + OD_LINE_WIDTH, 2);
}

void CFlownetController::SetPropertiesCNTLink(CODLinkComponent* pLinkComp)
{
	pLinkComp->SetType(_T("PID Link"));
	pLinkComp->SetName(_T("PID Link"));

	CODLineComponent* pLine = pLinkComp->GetLine();
	pLine->SetValue(OD_PROP_LINE + OD_LINE_COLOR, RGB(0,128,64));
	pLine->SetValue(OD_PROP_LINE + OD_LINE_WIDTH, 2);
}


// called from InsertSymbol() to insert nodes when the user selected a node 
// in the Outlook bar
void CFlownetController::InsertNodeComponent(DWORD dwResID)
{
	CODSymbolComponent* pNode = CreateNodeComponent(dwResID);
	//GetFlownetModel()->AddElementData(pNode);
	OnInsertSymbol(pNode);
}


// called from InsertSymbol() to insert elements when the user selected an element 
// in the Outlook bar
void CFlownetController::InsertElementComponent(DWORD dwResID)
{
	CODSymbolComponent* pElement = CreateElementComponent(dwResID);
//	if (pElement->GetType() != _T("HeatExchangerSecondaryElement") && pElement->GetType() != _T("Phantom"))
//	  GetFlownetModel()->AddElementData(pElement);
	OnInsertSymbol(pElement);
}


// called from CFlownetModel::CreateGraphicalLayout() to insert elements when importing
// from 5.x network files
void CFlownetController::InsertOldElementComponent(CElementComponent* pCompElem, int iXOffSet, int iYOffSet, int iScale)
{
	m_pInsert = pCompElem;
	CElementData* l_pElementData = (CElementData*)pCompElem->GetUserData();
	CGUIElement* l_pGUIElement = l_pElementData->getm_pGUIElement();

	// EvH I have added this logic!!!
	CreatePointAndInsertSymbol(	l_pGUIElement->getm_iXPos(), 
											l_pGUIElement->getm_iYPos(),
											iXOffSet,
											iYOffSet,
											iScale);
}

// called from CFlownetModel::CreateGraphicalLayout() to insert nodes when importing
// from 5.x network files
void CFlownetController::InsertOldNodeComponent(CNodeComponent* pCompElem, int iXOffSet, int iYOffSet, int iScale)
{
	m_pInsert = pCompElem;
	CNodeData* l_pNodeData = (CNodeData*)pCompElem->GetUserData();
	CNode* l_pNode = l_pNodeData->getm_pNode();

//	if ( l_pNode->getm_xPos() == 0 && l_pNode->getm_yPos() == 0 )
//		AfxMessageBox("Zero Coordinates");

	CreatePointAndInsertSymbol(	l_pNode->getm_xPos(),
											l_pNode->getm_yPos(),
											iXOffSet,
											iYOffSet,
											iScale);
}

// called from CFlownetModel to insert pipe or link elements from old files
void CFlownetController::InsertOldPipeOrLinkComponent(CODLinkComponent* pLinkComp, CODSymbolComponent* pSourceComp, CODSymbolComponent* pTargetComp)
{
	m_pSourcePort = pSourceComp->GetCenterPort();

	m_pTargetPort = pTargetComp->GetCenterPort();

	if ( m_pSourcePort != 0 && m_pTargetPort != 0 )
	{
		CPoint l_ptSourceLog = m_pSourcePort->GetLocation();
		CPoint l_ptTargetLog = m_pTargetPort->GetLocation();
		CPoint l_ptSourceDev = l_ptSourceLog;
		CPoint l_ptTargetDev = l_ptTargetLog;
		VpLPtoDP(&l_ptSourceDev);
		VpLPtoDP(&l_ptTargetDev);
		
		CODController::OnInsertLink(pLinkComp);
		CODController::LinkReady(0, l_ptSourceDev);
		m_arPointsLog.Add(l_ptTargetLog);

		m_pTargetPort = pTargetComp->GetCenterPort();

		CODController::EndLink(0, l_ptTargetDev);
	}
	else
	{
		AfxMessageBox("Source or target not found in CFlownetController::InsertOldPipeOrLinkComponent");
	}
}

void CFlownetController::CreatePointAndInsertSymbol(int iX, int iY, int iXOffSet, int iYOffSet, int iScale)
{
	CRect l_CRect = m_pInsert->GetBounds();
	CPoint l_Point(	(iX + iXOffSet) * iScale,
							(iY + iYOffSet) * iScale );
	InsertSymbol(0,l_Point);
}

void CFlownetController::InsertMasterSlaveLink(int iSlaveOID, int iMasterOID)
{
	CFlownetComponent* pSlave = FindComponentFromOID(iSlaveOID);
	CFlownetComponent* pMaster= FindComponentFromOID(iMasterOID);

	// Create an inert graphical link and connect the items
	if ((pSlave!=0)&&(pMaster!=0))
	{
		// get a hold of the element's center port
		CODPortComponent* pSlavePort  = pSlave->GetCenterPort();
		CODPortComponent* pMasterPort = pMaster->GetCenterPort();
		CPoint ptSlavePortLog  = pSlavePort->GetLocation();		// these points are actually still in logical units
		CPoint ptMasterPortLog = pMasterPort->GetLocation();		// these points are actually still in logical units
		CPoint ptSlavePortDev = ptSlavePortLog;
		CPoint ptMasterPortDev = ptMasterPortLog;
		VpLPtoDP(&ptSlavePortDev);										// convert now to device units
		VpLPtoDP(&ptMasterPortDev);										// convert now to device units
		
		// create link and connect between primary and secondary elements
		OnCreateNormalLink();
		SetPropertiesMasterSlaveLink((CODLinkComponent*)m_pInsert);
		CODController::LinkReady(0, ptSlavePortDev);
		m_arPointsLog.Add(ptMasterPortLog);
		m_pTargetPort = pMasterPort;
		CODController::EndLink(0, ptMasterPortDev);
	}

	GetFlownetModel()->SetModifiedFlag(TRUE);
}

void CFlownetController::RemoveMasterSlaveLink(int iSlaveOID, int iMasterOID)
{
	CFlownetComponent* pSlave = FindComponentFromOID(iSlaveOID);
	CFlownetComponent* pMaster= FindComponentFromOID(iMasterOID);

	if ((pSlave!=0)&&(pMaster!=0))
	{
		// We must find the master/slave link
		CODLinkComponent* pLink = 0;
		{
			CODEdgeArray Edges;
			IODEdge* pEdge = 0;
			// A slave can have only one master
			pSlave->GetEdgesLeaving(&Edges);
			int nSize = Edges.GetSize();
			for (int i = 0; i < nSize; ++i)
			{
				pEdge = Edges.GetAt(i);
				pLink = static_cast<CODLinkComponent*>(pEdge);
				if (pLink->GetType()=="MasterSlave Link")
				{
					break; // The link is found
				}
			}
		}
		
		// Now we have found the link we must remove it
		{
			// We must make a collection of links to delete
			CODComponentSet compSet;
			compSet.Add(pLink);

			CODController::ExecuteDeleteCommand(&compSet);
		}
	}
}

void CFlownetController::ShowComponents(BOOL bReveal, BOOL bMutualEx, const CString& sType, const CString& sGroup)
{
	// iterate through the element container to hide & show the controllers and the links connected to them
	CODComponent*        pComp = 0;
	CODComponentIterator iComps(GetFlownetModel()->GetComponents());
	IHideableComponent*  pHideComponent = 0;
	IHideableComponent*  pHideLink = 0;


	for (pComp = iComps.GetFirst(); pComp != NULL; pComp = iComps.GetNext())
	{
		if (pComp->GetType() == sType)
		{
			pHideComponent = dynamic_cast<IHideableComponent*>(pComp);

			if (sGroup == pHideComponent->GetGroupName() || sGroup == "All")
			{
				pHideComponent->Show(bReveal);
				// If we are an element we must show or hide the associated links
				pHideComponent->ShowAllLinks(bReveal);
			}
			else if (bMutualEx)
			{
				pHideComponent->Show(!bReveal);
				// If we are an element we must show or hide the associated links
				pHideComponent->ShowAllLinks(!bReveal);
			}
		}
	}

	GetCanvasVp()->UpdateAll(); // repaint
}

CFlownetComponent* CFlownetController::FindComponentFromOID(int iOID)
{
	CODComponentSet* l_pComponents = GetFlownetModel()->GetComponents();
	CODComponentIterator l_CompIterator(l_pComponents);
	CODComponent* l_pComp = l_CompIterator.GetFirst();

	CFlownetComponent* l_pResult = 0;
	CElementData* l_pElement = 0;
	for (; l_pComp!=0; l_pComp = l_CompIterator.GetNext())
	{
		if (l_pComp->IsKindOf(RUNTIME_CLASS(CElementComponent)))
		{
			l_pElement = static_cast<CElementData*>(l_pComp->GetUserData());
			int l_iElementOID = l_pElement->getm_iGUIElementOID();

			if (l_iElementOID == iOID)
			{
				l_pResult = static_cast<CFlownetComponent*>(l_pComp);
				break;
			}
		}
	}
	return l_pResult;
}

// This method has been added to prevent OLE drag-and-drop from taking place.  OLE drag-and-drop
// creates problems with the CODController::Moving() method not being called, which in turn is
// responsible for the alignment of the component to the grid.  We may need to modify this behaviour
// later if OLE bites us in the back in typical Microsoft fashion.
BOOL CFlownetController::IsOleInitialized()
{
	return FALSE; // TRUE;
}

// Called by InsertSymbol (etc)
void CFlownetController::AlignWithGrid(CODComponent* pComp, OD_CONTROL_POINT nAlignPoint)
{
	// call base class method with new alignment parameter
	CODController::AlignWithGrid(pComp, OD_CENTER);
}



/////////////////////////////////////////////////////////////////////////////
// CFlownetController message handlers

// Called when the user clicks on the link toolbar button to create a link
// There are two types of links:
//   - pipe links between two nodes;
//   - normal links in three flavours;
//     * between an element's port and a node;
//     * between a CHT (heat transfer) element's port and the centerport of any other element or pipe
//     * between a CNT (PID controller) element's port and the centerport of any other element or pipe
// Consequently, the link's type is assigned at run time - see EndLink() and different colours are assigned
void CFlownetController::OnCreateNormalLink() 
{
	CODLinkComponent* pLinkComp = CreateNormalLink();

	OnInsertLink(pLinkComp);
}

// Exactly the same as OnCreateLink(), but creates an orthogonal link
void CFlownetController::OnCreateOrthogonalLink() 
{
	CODLinkComponent* pLinkComp = CreateOrthogonalLink();
	OnInsertLink(pLinkComp);
}

void CFlownetController::CreateFlownetLink()
{
	if (m_dwResID == IDR_ORTHOGONAL_LINK)
	{
		OnCreateOrthogonalLink();
	}
	else
	{
		OnCreateNormalLink();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFlownetController virtual methods

// Called when the user clicks in the pane to start the drawing of a link
// by either clicking on the source node's port, or simply in empty space.
// When clicking in empty space, create a source node 
// if Tools|Customize|"Auto node creation in vacant space" is checked.
void CFlownetController::LinkReady(UINT nFlags, CPoint ptDev)
{
	CODLinkComponent* pLinkComp = (CODLinkComponent*)m_pInsert;
	ASSERT_VALID(pLinkComp);
	if (pLinkComp == NULL)
		return;

	CPoint ptLog = ptDev;
	VpDPtoLP(&ptLog);
	CODPortComponent* pPortHit = GetCanvasVp()->PortHitTest(ptLog, m_nPortHitTestSlop);

	// Link is being inserted into empty space. If auto node creation option is checked in Tools|Customize
	// then automatically create a node and attach the link to it
	if (pPortHit == 0)
	{
		if (m_pMainFrm->GetGUIPreferences().m_bAutoNodeCreation == TRUE)
		{
			m_pInsert = NULL;  // ensure that the operation to insert a node does not delete the link component
			InsertNodeComponent(IDR_NODE);
			InsertSymbol(0,ptDev);
			m_pInsert = pLinkComp; // restore the process of inserting the link component
		}
	}

	CODController::LinkReady(nFlags, ptDev);
}


// called when the user clicks in the pane to end the drawing of a link/pipe/CHT
// by either clicking on the destination node's port, 
// or by double-clicking in empty space.
// When double-clicking in empty space, create a target node (end node)
// if "Tools|Customize|Auto node creation in vacant space" is checked.

void CFlownetController::EndLink(UINT nFlags, CPoint ptDev)
{
	if (m_pInsert == 0)
	{
		ASSERT(FALSE); // TODO make sure this is never entered
		return;
	}
	
	CODLinkComponent* pLinkComp = (CODLinkComponent*)m_pInsert;
	ASSERT_VALID(pLinkComp);

	// get the source component and type
	CFlownetComponent* pSrcComp = (CFlownetComponent*) m_pSourcePort->GetOwner();
	CString sSrcType = (pSrcComp != 0)? pSrcComp->GetType() : _T("");


	// If link is being ended in space and auto-node creation is on, then create an endnode
	// the source node is at this stage already created, either by clicking in the component bar 
	// - see CFlownetController::InsertNodeComponent()
	// or on the fly when the pipe was clicked in empty space - see CFlownetController::LinkReady()
	if ( m_pTargetPort == 0 && m_pMainFrm->GetGUIPreferences().m_bAutoNodeCreation == TRUE)
	{ 
		if (sSrcType == _T("Node"))
		{
			AfxMessageBox(_T("Cannot connect two nodes!\nThis action will be cancelled."));
			CancelLink(nFlags, ptDev);
			CreateFlownetLink();
			return;
		}
		CPoint ptLog = ptDev;
		VpDPtoLP(&ptLog);
		m_pInsert = NULL;  // ensure that the operation to insert a node does not delete the link component
		InsertNodeComponent(IDR_NODE);
		m_pTargetPort = ((CODSymbolComponent*)m_pInsert)->GetCenterPort();
		InsertSymbol(0,ptDev);
		m_pInsert = pLinkComp; // restore the process of inserting the link component
	}
	else if (m_pTargetPort == 0 && m_pMainFrm->GetGUIPreferences().m_bAutoNodeCreation == FALSE)
	{
		CancelLink(nFlags, ptDev); // cannot end a link in empty space
	}
	// if auto-node creation was not on, cancel the action and return
	if (m_pTargetPort == 0)
	{
		CODController::EndLink(nFlags, ptDev);
		CreateFlownetLink();
		return;
	}

	// get the target component and type
	CFlownetComponent* pTgtComp = (CFlownetComponent*) m_pTargetPort->GetOwner();
	CString sTgtType = (pTgtComp != 0)? pTgtComp->GetType() : _T("");

	if (sSrcType == _T("SpecialConductiveHeatTransfer") ||
	    sTgtType == _T("SpecialConductiveHeatTransfer"))
	{
		SetPropertiesCHTLink(pLinkComp);
		CODController::EndLink(nFlags, ptDev);
		/*bool bRes = ConnectCHTorCNTLink(pLinkComp, pSrcComp, pTgtComp, CString("SpecialConductiveHeatTransfer") );

		if (!bRes)
		{
			AfxMessageBox(_T("You must insert a CHT link between a CHT element\nand a node, a pipe or another element's center port"));
			CancelLink(nFlags, ptDev);
			return;
		}*/
	}
	// for CNT element links...the link must connect to a CNT element on the one end
	// and the center port of any flow element on the other end
	else if (sSrcType == _T("SpecialPIDController") ||
	         sTgtType == _T("SpecialPIDController"))
	{
		SetPropertiesCNTLink(pLinkComp);
		CODController::EndLink(nFlags, ptDev);
		/*bool bRes = ConnectCHTorCNTLink(pLinkComp, pSrcComp, pTgtComp, CString("SpecialPIDController") );

		if (!bRes)
		{
			AfxMessageBox(_T("You must insert a CNT link between a CNT element\nand a node, a pipe or another element's center port"));
			CancelLink(nFlags, ptDev);
			return;
		}*/
	}
	// for CNT element links...the link must connect to a CNT element on the one end
	// and the center port of any flow element on the other end
	else if ( (sSrcType == _T("HeatExchanger") ||
				  sSrcType == _T("HeatExchangerGasLiquid") ||
				  sSrcType == _T("HeatExchangerRecuperator") ||
				  sSrcType == _T("HeatExchangerShellAndTube") )
			   && sTgtType == _T("HeatExchangerSecondaryElement")
			  )
	{
		SetPropertiesCHTLink(pLinkComp);
		CODController::EndLink(nFlags, ptDev);
	}

	// cannot connect two nodes!
	else if (sSrcType == _T("Node") && sTgtType == _T("Node"))
	{
		AfxMessageBox(_T("Illegal connection!\nThis action will be cancelled."));
		CancelLink(nFlags, ptDev);
	}
	else if ((sSrcType == "Node" && sTgtType == "Phantom")||
			(sSrcType == "Phantom" && sTgtType == "Node"))
	{
		// Color the node points
		SetPropertiesPhantomLink(pLinkComp);
		CODController::EndLink(nFlags, ptDev);
	}

	// for ordinary links...the link must connect to the port of an element 
	// on the one end and a node on the other
	else if(sSrcType == _T("Node") &&
	        pTgtComp->IsKindOf(RUNTIME_CLASS(CODSymbolComponent)) 
			  //&& m_pTargetPort != pTgtComp->GetCenterPort()
			  ||
	        sTgtType == _T("Node") &&
	        pSrcComp->IsKindOf(RUNTIME_CLASS(CODSymbolComponent)) 
			  //&& m_pSourcePort != pSrcComp->GetCenterPort() 
			  )
	{
		SetPropertiesNormalLink(pLinkComp);
		CODController::EndLink(nFlags, ptDev);
		/*bool bRes = ConnectNormalLink(pLinkComp, pSrcComp, pTgtComp);

		if (!bRes)
		{
			AfxMessageBox(_T("You must insert a link between an element and a node"));
			CancelLink(nFlags, ptDev);
			return;
		}*/
	}
	else
	{
		AfxMessageBox(_T("Illegal connection!\nThis action will be cancelled."));
		CancelLink(nFlags, ptDev);
	}

	CreateFlownetLink();
}


void CFlownetController::OnEditSelectAll() 
{
	CODController::SelectAll();
}

// this function is required to fix the ObjectiveViews left mouse click zoom-in bug!!
void CFlownetController::EndZoomRect(UINT nFlags, CPoint ptDev)  // virtual!
{
	// Release mouse capture
	ReleaseCapture();
	
	// Convert to logical coordinates
	POINT ptsLog[2];
	ptsLog[0] = m_ptOrigDev;
	ptsLog[1] = ptDev;
	VpDPtoLP(ptsLog, 2);
	
	// don't allow really small rectangle - ie if only a click and not a rectangle select
	CRect rectZoom(ptsLog[0], ptsLog[1]);
	if (rectZoom.Width() < 5 || rectZoom.Height() < 5)
	{
		ZoomIn(nFlags, ptDev); // just zoom in at point
		return;
	}
	// if a larger rectangle, is selected, go to default behaviour
	CODController::EndZoomRect(nFlags, ptDev);
}

void CFlownetController::OnSelectionChange(CODComponentSet* pChangedSet)
{
	if ( !CFlownetController::m_bImporting )
		CODController::OnSelectionChange(pChangedSet);
}


BOOL CFlownetController::HitComponent(UINT nFlags, CPoint ptLog, int nButton) 
{
	BOOL isHit = CODController::HitComponent(nFlags, ptLog, nButton);
	
	CFlownetComponent* pHit = (CFlownetComponent*) m_pCompHit;

// HvdW 18/12/2001:
// The following lines have been removed after we have changed the pipes from links to components.
// This was done so the property pages will be properly removed when links or secondary elements are selected.
/*	if ( (pHit != 0) && (pHit->GetUserData() == 0) ) // hit a component with no user data
		return isHit;
	
	if ( ((pHit != 0) && !pHit->IsKindOf(RUNTIME_CLASS(CODSymbolComponent))) || pHit == 0)  // hit a component that is not a symbol such as a text item
	{
		m_wndWorkspace.ReloadPropSheetsData(this, 0);  // no element with user data selected
		GetWnd()->SetFocus();  // set the focus back to the viewport, as ReloadPropSheetsData will put the focus on the propsheets.  If the focus is not set back to the viewport, one will have to select a component twice (to get the focus on the component) before the delete key will work
		return isHit;
	}*/
	
	m_wndWorkspace.ReloadPropSheetsData(this, m_pCompHit);
	GetWnd()->SetFocus();  // set the focus back to the viewport, as ReloadPropSheetsData will put the focus on the propsheets.  If the focus is not set back to the viewport, one will have to select a component twice (to get the focus on the component) before the delete key will work
	return isHit;
}

// Called when the user clicked on a component button in the Outlook shortcut bar
// all it does is to check the validity of pSymbolComponent, remove whatever 
// is connected to m_pInsert and then set m_pInsert equal to pSymbolComponent
void CFlownetController::OnInsertSymbol(CODSymbolComponent* pSymbolInsert)
{
	CODController::OnInsertSymbol(pSymbolInsert);
}

// Called when the user clicks on the pane to actually place a component on the drawing
void CFlownetController::InsertSymbol(UINT nFlags, CPoint ptDev)
{
	CODSymbolComponent* pComp = (CODSymbolComponent*)m_pInsert;
	BOOL bNodeElementNodeCreation = m_pMainFrm->GetGUIPreferences().m_bNodeElementNodeCreation;
	// This method may create additional components, so the original resource id of the element 
	// chosen by the user may be lost
	DWORD l_dwResID = m_dwResID;  

	// insert the element
	if ( !CFlownetController::m_bImporting )
		GetFlownetModel()->AddElementData(pComp);
	CODController::InsertSymbol(nFlags, ptDev); 
	// get the active controller set name
	CString sElemType = pComp->GetType();
	if (sElemType == _T("SpecialPIDController") )
	{
		CProjectTreeCtrl* pTreeCtrl = m_pMainFrm->GetProjectTree();
		CString sControlSetName = pTreeCtrl->GetActiveControlSetName();
		// store the controller set name in the element
		CElementData* pElemData = (CElementData*)pComp->GetUserData();
		pElemData->SetSetName(sControlSetName);
	}

	// get a hold of the element's center port
	CODPortComponent* pElemPort = pComp->GetCenterPort();
	CPoint ptElemPortLog = pElemPort->GetLocation();		// these points are actually still in logical units
	CPoint ptElemPortDev = ptElemPortLog;
	VpLPtoDP(&ptElemPortDev);										// convert now to device units

	//--- for heat exchangers ----
	CString strType = pComp->GetType();
	int l_idrType = 0;
	int l_idrSecType = 0;

	if (strType == _T("HeatExchanger") || 
		strType == _T("HeatExchangerGasLiquid") ||
		 strType == _T("HeatExchangerRecuperator") ||
		 strType == _T("HeatExchangerShellAndTube") )
		l_idrSecType = IDR_HEATEXCHANGER_SECONDARY;

	// if it is a heat exchanger to be inserted, also insert and connect its secondary element
	if (l_idrSecType != 0 && !CFlownetController::m_bImporting )
	{
		// create and insert secondary element
		InsertElementComponent(l_idrSecType);
		CElementComponent* pSecElem = (CElementComponent*) m_pInsert;
		GetFlownetModel()->AddElementData(pSecElem);
		CODPortComponent* pSecElemPort = pSecElem->GetCenterPort();
		CSize cSize = m_pViewport->GetMinGridPelSpacing(); 
		CPoint ptInsert(ptDev.x , ptDev.y + cSize.cy*20);
		CODController::InsertSymbol(0,ptInsert);
		CPoint ptSecElemPortLog = pSecElemPort->GetLocation();	// these points are actually still in logical units
		CPoint ptSecElemPortDev = ptSecElemPortLog;
		VpLPtoDP(&ptSecElemPortDev);									// convert now to device units
		// create link and connect between primary and secondary elements
		OnCreateNormalLink();
		SetPropertiesCHTLink((CODLinkComponent*)m_pInsert);
		CODController::LinkReady(0, ptElemPortDev);
		m_arPointsLog.Add(ptSecElemPortLog);
		m_pTargetPort = pSecElemPort;
		CODController::EndLink(0, ptSecElemPortDev);

		if (bNodeElementNodeCreation)
		{
			// create the upstream node for secondary element
			InsertNodeComponent(IDR_NODE);
			CNodeComponent* pNode1 = (CNodeComponent*) m_pInsert;
			GetFlownetModel()->AddElementData(pNode1);
			CODPortComponent* pNode1Port = pNode1->GetCenterPort();
			CSize cSize = m_pViewport->GetMinGridPelSpacing(); 
			CPoint ptInsert1(ptSecElemPortDev.x - cSize.cx*20, ptSecElemPortDev.y);
			CODController::InsertSymbol(0,ptInsert1);
			CPoint ptNode1PortLog = pNode1Port->GetLocation();	// these points are actually still in logical units
			CPoint ptNode1PortDev = ptNode1PortLog;
			VpLPtoDP(&ptNode1PortDev);									// convert now to device units
			// create the downstream node
			InsertNodeComponent(IDR_NODE);
			CNodeComponent* pNode2 = (CNodeComponent*) m_pInsert;
			GetFlownetModel()->AddElementData(pNode2);
			CODPortComponent* pNode2Port = pNode2->GetCenterPort();
			CPoint ptInsert2(ptSecElemPortDev.x + cSize.cx*20, ptSecElemPortDev.y);
			CODController::InsertSymbol(0,ptInsert2);
			CPoint ptNode2PortLog = pNode1Port->GetLocation();	// these points are actually still in logical units
			CPoint ptNode2PortDev = ptNode2PortLog;
			VpLPtoDP(&ptNode2PortDev);									// convert now to device units
			// create upstream link and connect to upstream node and element
			OnCreateNormalLink();
			SetPropertiesNormalLink((CODLinkComponent*)m_pInsert);
			CODController::LinkReady(0, ptNode1PortDev);
			m_arPointsLog.Add(ptSecElemPortLog);
			m_pTargetPort = pSecElemPort;
			CODController::EndLink(0, ptSecElemPortDev);
			// create downstream link and connect to element and downstream node  
			OnCreateNormalLink();
			SetPropertiesNormalLink((CODLinkComponent*)m_pInsert);
			CODController::LinkReady(0, ptSecElemPortDev);
			m_arPointsLog.Add(ptNode2PortLog);
			m_pTargetPort = pNode2Port;
			CODController::EndLink(0, ptNode2PortDev);
		}
	}
	

	// for elements with auto node-element-node creation option TRUE
	if ( pComp->GetType() != _T("Node")  && bNodeElementNodeCreation) 
	{
		if ( !CFlownetController::m_bImporting )
		{
			// create the upstream node
			InsertNodeComponent(IDR_NODE);
			CNodeComponent* pNode1 = (CNodeComponent*) m_pInsert;
			GetFlownetModel()->AddElementData(pNode1);
			CODPortComponent* pNode1Port = pNode1->GetCenterPort();
			CSize cSize = m_pViewport->GetMinGridPelSpacing(); 
			CPoint ptInsert1(ptDev.x - cSize.cx*20, ptDev.y);
			CODController::InsertSymbol(0,ptInsert1);
			CPoint ptNode1PortLog = pNode1Port->GetLocation();	// these points are actually still in logical units
			CPoint ptNode1PortDev = ptNode1PortLog;
			VpLPtoDP(&ptNode1PortDev);									// convert now to device units
			// create the downstream node
			InsertNodeComponent(IDR_NODE);
			CNodeComponent* pNode2 = (CNodeComponent*) m_pInsert;
			GetFlownetModel()->AddElementData(pNode2);
			CODPortComponent* pNode2Port = pNode2->GetCenterPort();
			CPoint ptInsert2(ptDev.x + cSize.cx*20, ptDev.y);
			CODController::InsertSymbol(0,ptInsert2);
			CPoint ptNode2PortLog = pNode1Port->GetLocation();	// these points are actually still in logical units
			CPoint ptNode2PortDev = ptNode2PortLog;
			VpLPtoDP(&ptNode2PortDev);									// convert now to device units
			// create upstream link and connect to upstream node and element
			OnCreateNormalLink();
			if (pComp->GetType() == _T("SpecialConductiveHeatTransfer"))
				SetPropertiesCHTLink((CODLinkComponent*)m_pInsert);
			else if (pComp->GetType() == _T("SpecialPIDController"))
				SetPropertiesCNTLink((CODLinkComponent*)m_pInsert);
			else
				SetPropertiesNormalLink((CODLinkComponent*)m_pInsert);
			CODController::LinkReady(0, ptNode1PortDev);
			m_arPointsLog.Add(ptElemPortLog);
			m_pTargetPort = pElemPort;
			CODController::EndLink(0, ptElemPortDev);
			// create downstream link and connect to element and downstream node  
			OnCreateNormalLink();
			if (pComp->GetType() == _T("SpecialConductiveHeatTransfer"))
				SetPropertiesCHTLink((CODLinkComponent*)m_pInsert);
			else if (pComp->GetType() == _T("SpecialPIDController"))
				SetPropertiesCNTLink((CODLinkComponent*)m_pInsert);
			else
				SetPropertiesNormalLink((CODLinkComponent*)m_pInsert);
			CODController::LinkReady(0, ptElemPortDev);
			m_arPointsLog.Add(ptNode2PortLog);
			m_pTargetPort = pNode2Port;
			CODController::EndLink(0, ptNode2PortDev);
			// connect the nodes to the element
			CNode* pCNode1 = pNode1->GetNode();
			CNode* pCNode2 = pNode2->GetNode();
			((CElementComponent*)pComp)->SetNode(pCNode1, nnNode1);
			((CElementComponent*)pComp)->SetNode(pCNode2, nnNode2);
			// set the element as the selected component
			ClearSelection();
			CODComponentSet setUpdate;
			setUpdate.Add(pComp);
			m_setSelection.Add(pComp);
			OnSelectionChange(&setUpdate);
			m_wndWorkspace.ReloadPropSheetsData(this, NULL);
			m_wndWorkspace.ReloadPropSheetsData(this, pComp);
			m_state = OD_SELECT_READY;
		}
	}

	if ( !CFlownetController::m_bImporting )
	{
		m_wndWorkspace.ReloadPropSheetsData(this, pComp);

		// Restore the resource id that may have been changed by programmatically creating components.
		// See the top of this method.
		m_dwResID = l_dwResID;
		// We are going to operate in repeat mode by default so we must create a 
		// new item to be put down immediately
		if (m_dwResID==IDR_NODE)
		{
			InsertNodeComponent(m_dwResID);
		}
		else
		{
			InsertElementComponent(m_dwResID);
		}
	}
}


CODInsertCommand* CFlownetController::ExecuteInsertCommand(CODComponent* pComp,
																			  const BOOL bAllowDuplicates)
{
	CODInsertCommand* pCmd = CODController::ExecuteInsertCommand(pComp, bAllowDuplicates);
	return pCmd;
}

CODInsertCommand* CFlownetController::ExecuteInsertCommand(CODComponentSet* pCompSet,
															const BOOL bAllowDuplicates)
{
	CODInsertCommand* pCmd = CODController::ExecuteInsertCommand(pCompSet, bAllowDuplicates);
	
	if (pCompSet != 0)
	{
		GetFlownetModel()->SetModifiedFlag(TRUE);
		CODComponent* pComp;
		CODComponentIterator CompIterator(pCompSet);
		CString l_sType;
		// First iterate through the nodes
		for (pComp = CompIterator.GetFirst(); pComp != NULL; pComp = CompIterator.GetNext())
		{
			CString l_sType = pComp->GetType(); // get by value only once
			if (	l_sType == _T("Normal Link") ||
					l_sType == _T("CHT Link")    ||
					l_sType == _T("PID Link") )
				continue;

			if (l_sType == _T("Node"))
			{
				GetFlownetModel()->RefreshNodeDataAtPaste(pComp);
			}
		}

		// now iterate through the Element excluding Controllers & CHTElements
		for (pComp = CompIterator.GetFirst(); pComp != NULL; pComp = CompIterator.GetNext())
		{
			CString l_sType = pComp->GetType(); // get by value only once
			if (	l_sType == _T("Normal Link")	||
					l_sType == _T("CHT Link")		||
					l_sType == _T("PID Link")		|| 
					l_sType == _T("Node") )
				continue;

			if (	l_sType != _T("SpecialConductiveHeatTransfer") &&
					l_sType != _T("SpecialPIDController") )
			{
				GetFlownetModel()->RefreshElementDataAtPaste(pComp, false);
			}
		}

		// Finaly iterate through the Controllers & CHTElements
		for (pComp = CompIterator.GetFirst(); pComp != NULL; pComp = CompIterator.GetNext())
		{
			CString l_sType = pComp->GetType(); // get by value only once
			if (	l_sType == _T("Normal Link")	||
					l_sType == _T("CHT Link")		||
					l_sType == _T("PID Link")		|| 
					l_sType == _T("Node") )
				continue;

			if (	l_sType == _T("SpecialConductiveHeatTransfer") ||
					l_sType == _T("SpecialPIDController") )
			{
				GetFlownetModel()->RefreshElementDataAtPaste(pComp, true);
			}
		}
	}
	return pCmd;
	//	return CODController::ExecuteInsertCommand(pCompSet,bAllowDuplicates);
	//return 0;  
	// CODInsertCommand::Execute returns Non-zero if the command was successful.
	// for some unknown reason says the Stringray docs that
}  // CODController::ExecuteInsertCommand returns a void!!



CODDeleteCommand* CFlownetController::ExecuteDeleteCommand(CODComponentSet* pCompSet)
{
	CODComponentIterator i(pCompSet);
	CODComponent* pComp;
	
	for (pComp = i.GetFirst(); pComp != NULL; pComp = i.GetNext())
	{
		CObject* l_pData = pComp->GetUserData(); 
		if (pComp->GetType() == _T("Normal Link"))
			((CFlownetLink*)pComp)->DisconnectNormalLink();
		else if (pComp->GetType() == _T("CHT Link") || pComp->GetType() == _T("PID Link"))
			((CFlownetLink*)pComp)->DisconnectCHTorCNTLink();
		else if (pComp->GetType() == _T("HeatExchangerSecondaryElement"))
		{
			DeleteAllLinksFromComponent(pComp);
			int l_ElemId = 0;
			pComp->GetValue(PROPERTY_ELEMENT_ID, l_ElemId);
			((CFlownetModel*)GetCanvasModel())->UnselectElemUID(l_ElemId); // so this number can be re-used!
		}
		else if ( l_pData != 0 )
		{
			DeleteAllLinksFromComponent(pComp);
			if ( pComp->GetType() == _T("Node") )
			{
				CNode* l_pNode = ((CNodeData*)l_pData)->getm_pNode();
				if (l_pNode != 0)
					((CFlownetModel*)GetCanvasModel())->UnselectNodeUID(l_pNode->getm_iUnum()); // so this number can be re-used!

				l_pNode->Delete();
				
				((CFlownetModel*)GetCanvasModel())->m_pFNClass->GetDBNodes()->erase(l_pNode);
				
				delete l_pNode;
				l_pNode = 0;
				m_wndWorkspace.m_pInputsFormView->m_pCurrentComponent = 0; // set the pointer to the component currently displayed in the formviews to 0 so that no attempt is made to save this deleted component!
				m_wndWorkspace.ReloadPropSheetsData(this, NULL);       // NULL == the component selected to be displayed - here none is to be displayed since it was deleted!
			}
			else
			{
				CGUIElement* l_pElement = ((CElementData*)l_pData)->getm_pGUIElement();
				if (l_pElement != 0)
					((CFlownetModel*)GetCanvasModel())->UnselectElemUID(l_pElement->GetUserNum());

				l_pElement->Delete();
				
				((CFlownetModel*)GetCanvasModel())->m_pFNClass->GetGUIElements()->erase(l_pElement);
				
				delete l_pElement;
				l_pElement = 0;
				m_wndWorkspace.m_pInputsFormView->m_pCurrentComponent = 0; // set the pointer to the component currently displayed in the formviews to 0 so that no attempt is made to save this deleted component!
				m_wndWorkspace.ReloadPropSheetsData(this, NULL);       // NULL == the component selected to be displayed - here none is to be displayed since it was deleted!
			}
		}
	}

	return CODController::ExecuteDeleteCommand(pCompSet);
}

// This method removes all the links from the given component.  However, it first 
// disconnects all the links from this component before the links are packed into an array 
// deleted as a group
CODDeleteCommand* CFlownetController::DeleteAllLinksFromComponent(CODComponent* pComp)
{
	CODEdgeArray l_IEdgesEntering;
	CODEdgeArray l_IEdgesLeaving;
	int iNumEdgesEntering = 0;
	int iNumEdgesLeaving  = 0;
	// get all the links	
	iNumEdgesEntering = ((CODSymbolComponent*)pComp)->GetEdgesEntering(&l_IEdgesEntering);
	iNumEdgesLeaving  = ((CODSymbolComponent*)pComp)->GetEdgesLeaving(&l_IEdgesLeaving);

	// Unbelievable, but CODEdgeArray does not have iterators!
	// build an array of all the links
	IODEdge* pIEdge = 0;
	CODComponentSet compSet;
	int nSize = l_IEdgesEntering.GetSize();
	for (int i = 0; i < nSize; i++)
	{
		pIEdge = l_IEdgesEntering.GetAt(i);
		compSet.Add(static_cast<CODLinkComponent*>(pIEdge) );
	}
	nSize = l_IEdgesLeaving.GetSize();
	for (i = 0; i < nSize; i++ )
	{
		pIEdge = l_IEdgesLeaving.GetAt(i);
		compSet.Add(static_cast<CODLinkComponent*>(pIEdge) );
	}

	// now disconnect all the links from the component
	((CFlownetComponent*)pComp)->DisconnectAllLinksFromComponentAndUpdate();
	// finally, delete all the links that previously was connected to the component

	return CODController::ExecuteDeleteCommand(&compSet);
}

/*
void CFlownetController::OnEditClear()
{
	CNodeData::m_bClipboardSerialize = true;
	CODController::OnEditClear();
	CNodeData::m_bClipboardSerialize = false;
}
*/

void CFlownetController::OnEditCut() 
{
	m_bClipboardSerialize = true;
	m_bClipboardCopy = false;
	CODController::OnEditCut();
	m_bClipboardSerialize = false;
}

void CFlownetController::OnEditCopy() 
{
	m_bClipboardSerialize = true;
	m_bClipboardCopy = true;
	CODController::OnEditCopy();
	m_bClipboardSerialize = false;
}

void CFlownetController::OnEditPaste() 
{
	m_bClipboardSerialize = true;
	CODController::OnEditPaste();  // this will call the virtual CFlownetController::ExecuteInsertCommand in this file
	m_bClipboardSerialize = false;
	m_bClipboardCopy = false;
}

void CFlownetController::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
  // To do Clipboard operations, first we must open the clipboard...
  OpenClipboard();

  // After opening we take a peek at the kind of data that is there.
  // If we have a metafile then we cannot allow pasting
  HANDLE hMetafile = ::GetClipboardData(CF_ENHMETAFILE);

  // Are we allowed to paste the type of content that we see?
  if (0!=hMetafile)
  {
//    ::MessageBeep(MB_OK);
//      pCmdUI->Enable(FALSE);
      ::CloseClipboard();
      return;
  }

//  pCmdUI->Enable();
/*
  if ((0!=::GetClipboardData(SomeIllegalType))||
      (0!=::GetClipboardData(SomeOtherIllegalType)))
  {
      ::CloseClipboard();
      return;  
  }
*/
  // Do a similar thing for other types of data that you don't want
  // to allow pasting to the clipboard

  // Then at the end we close it.
  ::CloseClipboard();
}

void CFlownetController::OnDrawSelect()
{
	m_dwResID = 0;
	CODController::OnDrawSelect();
}

void CFlownetController::OnDrawText()
{
	m_dwResID = 0;
	CODController::OnDrawText();
}

void CFlownetController::OnUpdateLinkNormal(CCmdUI *pCmdUI)
{
	CODEditProperties* pModelEditProps = (CODEditProperties*)GetCanvasModel()->GetProperty(OD_PROP_EDIT);
	pCmdUI->Enable(GetCanvasModel()->GetDesignMode() || !pModelEditProps->IsReadOnly());
	int nState = GetState();
	pCmdUI->SetCheck(m_dwResID == IDR_NORMAL_LINK && (nState == OD_LINK_READY || nState == OD_LINK_DRAW));
}

void CFlownetController::OnUpdateLinkOrthogonal(CCmdUI *pCmdUI)
{
	CODEditProperties* pModelEditProps = (CODEditProperties*)GetCanvasModel()->GetProperty(OD_PROP_EDIT);
	pCmdUI->Enable(GetCanvasModel()->GetDesignMode() || !pModelEditProps->IsReadOnly());
	int nState = GetState();
	pCmdUI->SetCheck(m_dwResID == IDR_ORTHOGONAL_LINK && (nState == OD_LINK_READY || nState == OD_LINK_DRAW));

}

void CFlownetController::OnTogglePhantomElements()
{
	m_bPhantomsVisible = !m_bPhantomsVisible;
	ShowComponents(m_bPhantomsVisible,FALSE,"Phantom");
}

void CFlownetController::OnUpdateTogglePhantomElements(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bPhantomsVisible);
}

