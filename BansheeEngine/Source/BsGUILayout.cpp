#include "BsGUILayout.h"
#include "BsGUIElement.h"
#include "BsGUILayoutX.h"
#include "BsGUILayoutY.h"
#include "BsGUISpace.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUILayout::GUILayout()
		:mIsDirty(false), mOptimalWidth(0), mOptimalHeight(0)
	{

	}

	GUILayout::~GUILayout() 
	{
		for(auto& child : mChildren)
		{
			if(child.isLayout())
			{
				// Child layouts are directly owned by us
				CM_DELETE(child.layout, GUILayout, PoolAlloc);
			}
			else if(child.isFixedSpace())
			{
				CM_DELETE(child.space, GUIFixedSpace, PoolAlloc);
			}
			else if(child.isFlexibleSpace())
			{
				CM_DELETE(child.flexibleSpace, GUIFlexibleSpace, PoolAlloc);
			}
			else
			{
				child.element->_setParentLayout(nullptr);
			}
		}
	}

	void GUILayout::addElement(GUIElement* element)
	{
		if(element->_getParentLayout() != nullptr)
			element->_getParentLayout()->removeElement(element);

		GUILayoutEntry entry;
		entry.setElement(element);

		element->_setParentLayout(this);
		mChildren.push_back(entry);
		mIsDirty = true;
	}

	void GUILayout::removeElement(GUIElement* element)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isElement() && child.element == element)
			{
				mChildren.erase(iter);
				foundElem = true;
				mIsDirty = true;
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	void GUILayout::insertElement(UINT32 idx, GUIElement* element)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		if(element->_getParentLayout() != nullptr)
			element->_getParentLayout()->removeElement(element);

		GUILayoutEntry entry;
		entry.setElement(element);

		element->_setParentLayout(this);
		mChildren.insert(mChildren.begin() + idx, entry);
		mIsDirty = true;
	}

	GUILayout& GUILayout::addLayoutX()
	{
		GUILayoutEntry entry;
		entry.setLayout(CM_NEW(GUILayoutX, PoolAlloc) GUILayoutX());

		mChildren.push_back(entry);
		mIsDirty = true;

		return *entry.layout;
	}

	GUILayout& GUILayout::addLayoutY()
	{
		GUILayoutEntry entry;
		entry.setLayout(CM_NEW(GUILayoutY, PoolAlloc) GUILayoutY());

		mChildren.push_back(entry);
		mIsDirty = true;

		return *entry.layout;
	}

	void GUILayout::removeLayout(GUILayout& layout)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isLayout() && child.layout == &layout)
			{
				CM_DELETE(child.layout, GUILayout, PoolAlloc);

				mChildren.erase(iter);
				foundElem = true;
				mIsDirty = true;
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	GUILayout& GUILayout::insertLayoutX(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutEntry entry;
		entry.setLayout(CM_NEW(GUILayoutX, PoolAlloc) GUILayoutX());

		mChildren.insert(mChildren.begin() + idx, entry);
		mIsDirty = true;

		return *entry.layout;
	}

	GUILayout& GUILayout::insertLayoutY(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutEntry entry;
		entry.setLayout(CM_NEW(GUILayoutY, PoolAlloc) GUILayoutY());;

		mChildren.insert(mChildren.begin() + idx, entry);
		mIsDirty = true;

		return *entry.layout;
	}

	GUIFixedSpace& GUILayout::addSpace(UINT32 size)
	{
		GUILayoutEntry entry;
		entry.setSpace(CM_NEW(GUIFixedSpace, PoolAlloc) GUIFixedSpace(size));

		mChildren.push_back(entry);
		mIsDirty = true;

		return *entry.space;
	}

	void GUILayout::removeSpace(GUIFixedSpace& space)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isFixedSpace() && child.space == &space)
			{
				CM_DELETE(child.space, GUIFixedSpace, PoolAlloc);

				mChildren.erase(iter);
				foundElem = true;
				mIsDirty = true;
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	GUIFixedSpace& GUILayout::insertSpace(UINT32 idx, UINT32 size)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutEntry entry;
		entry.setSpace(CM_NEW(GUIFixedSpace, PoolAlloc) GUIFixedSpace(size));

		mChildren.insert(mChildren.begin() + idx, entry);
		mIsDirty = true;

		return *entry.space;
	}

	GUIFlexibleSpace& GUILayout::addFlexibleSpace()
	{
		GUILayoutEntry entry;
		entry.setFlexibleSpace(CM_NEW(GUIFlexibleSpace, PoolAlloc) GUIFlexibleSpace());

		mChildren.push_back(entry);
		mIsDirty = true;

		return *entry.flexibleSpace;
	}

	void GUILayout::removeFlexibleSpace(GUIFlexibleSpace& space)
	{
		bool foundElem = false;
		for(auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
		{
			GUILayoutEntry& child = *iter;

			if(child.isFlexibleSpace() && child.flexibleSpace == &space)
			{
				CM_DELETE(child.flexibleSpace, GUIFlexibleSpace, PoolAlloc);

				mChildren.erase(iter);
				foundElem = true;
				mIsDirty = true;
				break;
			}
		}

		if(!foundElem)
			CM_EXCEPT(InvalidParametersException, "Provided element is not a part of this layout.");
	}

	GUIFlexibleSpace& GUILayout::insertFlexibleSpace(UINT32 idx)
	{
		if(idx < 0 || idx >= (UINT32)mChildren.size())
			CM_EXCEPT(InvalidParametersException, "Index out of range: " + toString(idx) + ". Valid range: 0 .. " + toString((UINT32)mChildren.size()));

		GUILayoutEntry entry;
		entry.setFlexibleSpace(CM_NEW(GUIFlexibleSpace, PoolAlloc) GUIFlexibleSpace());

		mChildren.insert(mChildren.begin() + idx, entry);
		mIsDirty = true;

		return *entry.flexibleSpace;
	}

	UINT32 GUILayout::getNumChildren() const
	{
		return (UINT32)mChildren.size();
	}

	void GUILayout::_update(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth)
	{
		updateOptimalSizes(); // We calculate optimal sizes of all layouts as a pre-processing step, as they are requested often during update
		updateInternal(x, y, width, height, depth);
		mIsDirty = false;
	}

	bool GUILayout::_isDirty()
	{
		if(mIsDirty)
			return true;

		for(auto& child : mChildren)
		{
			if(child.isLayout())
			{
				if(child.layout->_isDirty())
					return true;
			}
		}

		return false;
	}
}