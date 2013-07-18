#include "BsGUIElement.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIElement::GUIElement(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions, bool acceptsKeyboardFocus)
		:mParent(parent), mLayoutOptions(layoutOptions), mWidth(0), mHeight(0), mDepth(0), mStyle(style),
		mAcceptsKeyboardFocus(acceptsKeyboardFocus), mParentLayout(nullptr)
	{
		mParent.registerElement(this);
	}

	GUIElement::~GUIElement()
	{
		if(mParentLayout != nullptr)
			mParentLayout->removeElement(this);
	}

	void GUIElement::updateRenderElements()
	{
		updateRenderElementsInternal();
		_markAsClean();
	}

	void GUIElement::setLayoutOptions(const GUILayoutOptions& layoutOptions) 
	{
		if(layoutOptions.maxWidth < layoutOptions.minWidth)
		{
			CM_EXCEPT(InvalidParametersException, "Maximum width is less than minimum width! Max width: " + 
			toString(layoutOptions.maxWidth) + ". Min width: " + toString(layoutOptions.minWidth));
		}

		if(layoutOptions.maxHeight < layoutOptions.minHeight)
		{
			CM_EXCEPT(InvalidParametersException, "Maximum height is less than minimum height! Max height: " + 
			toString(layoutOptions.maxHeight) + ". Min height: " + toString(layoutOptions.minHeight));
		}

		mLayoutOptions = layoutOptions; 
	}


	bool GUIElement::mouseEvent(const GUIMouseEvent& ev)
	{
		return false;
	}

	bool GUIElement::keyEvent(const GUIKeyEvent& ev)
	{
		return false;
	}

	bool GUIElement::commandEvent(const GUICommandEvent& ev)
	{
		return false;
	}

	void GUIElement::_setWidgetDepth(UINT8 depth) 
	{ 
		mDepth |= depth << 24; 
		markMeshAsDirty();
	}

	void GUIElement::_setAreaDepth(UINT16 depth) 
	{ 
		mDepth |= depth << 8; 
		markMeshAsDirty();
	}

	void GUIElement::_setOffset(const CM::Int2& offset) 
	{ 
		if(mOffset != offset)
			markMeshAsDirty();

		mOffset = offset;
	}

	void GUIElement::_setWidth(UINT32 width) 
	{ 
		if(mWidth != width)
			markContentAsDirty();

		mWidth = width; 
	}

	void GUIElement::_setHeight(UINT32 height) 
	{ 
		if(mHeight != height)
			markContentAsDirty();

		mHeight = height;
	}

	void GUIElement::_setClipRect(const CM::Rect& clipRect) 
	{ 
		if(mClipRect != clipRect)
			markMeshAsDirty();

		mClipRect = clipRect; 
	}

	Rect GUIElement::getVisibleBounds() const
	{
		Rect bounds = _getBounds();
		
		bounds.x += mStyle->margins.left;
		bounds.y += mStyle->margins.top;
		bounds.width = (UINT32)std::max(0, (INT32)bounds.width - (INT32)(mStyle->margins.left + mStyle->margins.right));
		bounds.height = (UINT32)std::max(0, (INT32)bounds.height - (INT32)(mStyle->margins.top + mStyle->margins.bottom));

		return bounds;
	}

	Rect GUIElement::getContentBounds() const
	{
		Rect bounds;

		bounds.x = mOffset.x + mStyle->margins.left + mStyle->contentOffset.left;
		bounds.y = mOffset.y + mStyle->margins.top + mStyle->contentOffset.top;
		bounds.width = (UINT32)std::max(0, (INT32)mWidth - 
			(INT32)(mStyle->margins.left + mStyle->margins.right + mStyle->contentOffset.left + mStyle->contentOffset.right));
		bounds.height = (UINT32)std::max(0, (INT32)mHeight - 
			(INT32)(mStyle->margins.top + mStyle->margins.bottom + mStyle->contentOffset.top + mStyle->contentOffset.bottom));

		return bounds;
	}

	bool GUIElement::_isInBounds(const CM::Int2 position) const
	{
		Rect contentBounds = getVisibleBounds();

		return contentBounds.contains(position);
	}

	GUILayoutOptions GUIElement::getDefaultLayoutOptions(const GUIElementStyle* style)
	{
		GUILayoutOptions layoutOptions;

		layoutOptions.fixedWidth = style->fixedWidth;
		layoutOptions.fixedHeight = style->fixedHeight;
		layoutOptions.width = style->width;
		layoutOptions.height = style->height;
		layoutOptions.minWidth = style->minWidth;
		layoutOptions.maxWidth = style->maxWidth;
		layoutOptions.minHeight = style->minHeight;
		layoutOptions.maxHeight = style->maxHeight;

		return layoutOptions;
	}

	void GUIElement::_destroyInternal(GUIElement* element)
	{
		cm_delete<PoolAlloc>(element);
	}

	void GUIElement::destroy(GUIElement* element)
	{
		element->mParent.unregisterElement(element);

		cm_delete<PoolAlloc>(element);
	}
}