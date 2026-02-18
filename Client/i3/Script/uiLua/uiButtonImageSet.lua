require "Script.uiLua.uiCaption"

class 'uiButtonImageSet' (uiCaption)

function uiButtonImageSet:__init(id)
	uiCaption.__init(self, id)
end

function uiButtonImageSet:setShape( Idx)
	uiImageSet_setShape( self.ctrlID, Idx)
end

function uiButtonImageSet:setShapeByString( str)
	uiImageSet_setShapeByString( self.ctrlID, str)
end

function uiButtonImageSet:getShapeIndex( )
	return uiImageSet_getShapeIndex( self.ctrlID)
end

function uiButtonImageSet:getShapeString( )
	return uiImageSet_getShapeString( self.ctrlID)
end

function uiButtonImageSet:setOnlyView( val)
    uiImageSet_setViewOnly( self.ctrlID, val)
end