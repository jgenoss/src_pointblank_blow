--캡션
require "Script.uiLua.uiControl"

class 'uiCaption' (uiControl)

function uiCaption:__init(id)
	uiControl.__init(self, id)

	self.initstr = nil
	self.txtSpaceX = 0
	self.txtSpaceY = 0
end

function uiCaption:recordInitialState()
	uiControl.recordInitialState(self)

	self.initstr = self:getText()
	self.txtSpaceX, self.txtSpaceY = self:getTextSpace()
end

function uiCaption:restoreInitialState()
	uiControl.restoreInitialState(self)

	if( self.initstr ~= nil) then
		self:setText( self.initstr)
		self:setTextSpace( self.txtSpaceX, self.txtSpaceY)
	end
end

function uiCaption:getText()
	local str = uiCaption_getText( self.ctrlID)
	return str
end

function uiCaption:setText( str)
	uiCaption_setText(self.ctrlID, str)
end

function uiCaption:setTextPtr(str)
	uiCaption_setTextPtr(self.ctrlID, str)
end

function uiCaption:getTextLen()
	return uiCaption_getTextLen( self.ctrlID)
end

function uiCaption:getTextExtentX()
	return uiCaption_getTextExtentX(self.ctrlID)
end

function uiCaption:setMaxLetters( maxcount)
	return uiCaption_setMaxLetters(self.ctrlID, maxcount)
end

-- 최대 버퍼 크기 반환
function uiCaption:getMaxLetters()
	return uiCaption_getMaxLetters(self.ctrlID)
end

-- 글자 제한 수 반환
function uiCaption:getLetterLimit()
	return uiCaption_getLetterLimit(self.ctrlID)
end

function uiCaption:setStyle(style)
	uiCaption_setStyle(self.ctrlID, style)
end

function uiCaption:getStyle()
	return uiCaption_getStyle(self.ctrlID)
end

--font
function uiCaption:getFont()
	return uiCaption_getFont(self.ctrlID)
end

function uiCaption:setFont(name, size, weight)
	uiCaption_setFont(self.ctrlID, name, size, weight)
end

function uiCaption:getFontName()
	return uiCaption_getFontName(self.ctrlID)
end

function uiCaption:setFontName(name)
	uiCaption_setFontName(self.ctrlID, name)
end

function uiCaption:getFontSize()
	return uiCaption_getFontSizePt(self.ctrlID)
end

function uiCaption:setFontSize(size)
	uiCaption_setFontSizePt(self.ctrlID, size)
end

function uiCaption:getFontWeight()
	return uiCaption_getFontWeight(self.ctrlID)
end

function uiCaption:setFontWeight(weight)
	uiCaption_setFontWeight(self.ctrlID, weight)
end

--text align
function uiCaption:getTextAlign()
	return uiCaption_getTextAlign(self.ctrlID)
end

function uiCaption:setTextAlign(alignX, alignY)
	uiCaption_setTextAlign(self.ctrlID, alignX, alignY)
end

function uiCaption:setTextAlignX(alignX)
	uiCaption_setTextAlignX(self.ctrlID, alignX)
end

function uiCaption:getTextAlignX()
	return uiCaption_getTextAlignX(self.ctrlID)
end

function uiCaption:getTextAlignY()
	return uiCaption_getTextAlignY(self.ctrlID)
end

function uiCaption:setTextAlignY(alignY)
	uiCaption_setTextAlignY(self.ctrlID, alignY)
end

-- text space
function uiCaption:getTextSpace()
	return uiCaption_getTextSpace(self.ctrlID)
end

function uiCaption:setTextSpace(spacex, spacey)
	uiCaption_setTextSpace(self.ctrlID, spacex, spacey)
end

function uiCaption:getTextSpaceX()
	return uiCaption_getTextSpaceX(self.ctrlID)
end

function uiCaption:setTextSpaceX(spacex)
	uiCaption_setTextSpaceX(self.ctrlID, spacex)
end

function uiCaption:getTextSpaceY()
	return uiCaption_getTextSpaceY(self.ctrlID)
end

function uiCaption:setTextSpaceY(spaceY)
	uiCaption_setTextSpaceY(self.ctrlID, spaceY)
end

-- text color
function uiCaption:getTextColor()
	return uiCaption_getTextColor(self.ctrlID)
end

function uiCaption:setTextColor(red,green,blue,alpha)
	uiCaption_setTextColor(self.ctrlID, red, green, blue, alpha)
end

function uiCaption:setTextColor4Byte(rgba)
	uiCaption_setTextColor4Byte(self.ctrlID, rgba) 
end

-- text shadow
function uiCaption:getTextShadowColor()
	return uiCaption_getTextShadowColor(self.ctrlID)
end

function uiCaption:setTextShadowColor(red,green,blue,alpha)
	uiCaption_setTextShadowColor(self.ctrlID, red, green, blue, alpha)
end

function uiCaption_getTextShadowQualityEnable()
	return uiCaption_getTextShadowQualityEnable()
end

function uiCaption:setTextShadowQuality(quality)
	uiCaption_setTextShadowQuality(self.ctrlID, quality)
end

function uiCaption:getTextShadowQuality()
	return uiCaption_getTextShadowQuality()
end

function uiCaption:setShadowType( typeShadow )
	uiCaption_setShadowType(self.ctrlID, typeShadow)
end

function uiCaption:setQualityType( typeQuality )
	uiCaption_setQualityType(self.ctrlID, typeQuality)
end

