class "uiAnim"

-- CubicHermite / Interpolate code snippet reference
-- http://stackoverflow.com/questions/3367308/math-ease-in-ease-out-a-displacement-using-hermite-curve-with-time-constraint

function uiAnim:CubicHermite(t, p0, p1, m0, m1) 
	t2 = t * t
	t3 = t2 * t
	return (2*t3 - 3*t2 + 1)*p0 + (t3-2*t2+t)*m0 + (-2*t3+3*t2)*p1 + (t3-t2)*m1
end

-- Ease-In & Ease-Out
function uiAnim:EaseInOut( timeToAccel, timeCruising, timeToDecel, finalPosition, currentTime) 
	t1 = timeToAccel
	t2 = timeCruising
	t3 = timeToDecel
	x = finalPosition
	t = currentTime

	v = x / (t1/2 + t2 + t3/2)
	x1 = v * t1 / 2
	x2 = v * t2
	x3 = v * t3 / 2

	if t <= t1 then
		return uiAnim:CubicHermite(t/t1, 0, x1, 0, x2/t2*t1)
	elseif t <= (t1+t2) then
		return x1 + x2 * (t-t1) / t2
	elseif t <= (t1+t2+t3) then
		return uiAnim:CubicHermite((t-t1-t2)/t3, x1+x2, x, x2/t2*t3, 0)
	else
		return x
	end
end

function uiAnim:Clamp( fVal, fFloor, fCeil)
	if fVal < fFloor then
		fVal = fFloor
	elseif fVal > fCeil then
		fVal = fCeil
	end
	
	return fVal
end

function uiAnim:SetAlpha( ctrl, fAlpha)
	fAlpha = uiAnim:Clamp( fAlpha, 0.0, 1.0)
	r,g,b = ctrl:getColor()
	nVal = fAlpha * 255
	ctrl:setColor( r,g,b, nVal)
end

function uiAnim:SetColor( ctrl, fR, fG, fB)
	fR = uiAnim:Clamp( fR, 0.0, 1.0)
	fG = uiAnim:Clamp( fG, 0.0, 1.0)
	fB = uiAnim:Clamp( fB, 0.0, 1.0)
	
	r,g,b,a = ctrl:getColor()
	r = fR * 255
	g = fG * 255
	b = fB * 255
	
	ctrl:setColor( r, g, b, a)
end



-- Editor Lua Script 상에서 사용하기 편하게 만든 Animation Preset.

function uiAnim:PopupAnim( ctrl, nX, nY, nOffset, fDuration, fCurTime, dt )
	if fCurTime <= fDuration then
		fCurTime = fCurTime + dt
		EaseIn = fDuration * 0.09
		Travel = fDuration * 0.01
		EaseOut = fDuration * 0.9
		fVal = uiAnim:EaseInOut( EaseIn , Travel, EaseOut, 1.0, fCurTime)
		ctrl:setPosition( nX,  (1.0 - fVal) * nOffset + nY, 0)
	else
		ctrl:setPosition( nX, nY, 0)
	end
	return fCurTime
end

function uiAnim:Flipper( Ctrl, nStartX, nSizeWidth, fRatio, bReverse)
	fRatio = uiAnim:Clamp( fRatio, 0.0, 1.0)

	if bReverse == true then
		fRatio = 1.0 - fRatio
	end

	x,y = Ctrl:getSize()
	Ctrl:setSize( nSizeWidth * fRatio, y)

	x,y = Ctrl:getPosition()

	AddX = nSizeWidth * 0.5 * (1.0 - fRatio)
	Ctrl:setPosition(nStartX + AddX, y)
end

function uiAnim:FlipperSetHalf( ctrlA, ctrlB, nStartX, nSizeWidth, fCurTime, fDuration)
	fCurTime = uiAnim:Clamp( fCurTime, 0.0, fDuration)
	
	EaseValue = fDuration * 0.45
	Travel = fDuration * 0.1
	
	fRatio = uiAnim:EaseInOut( EaseValue, Travel, EaseValue, 1.0, fCurTime)
	
	if fRatio <= 0.5 then
		uiAnim:Flipper( ctrlA, nStartX, nSizeWidth, fRatio*2, true)
		uiAnim:Flipper( ctrlB, nStartX, nSizeWidth, 0.0, false)
	elseif fRatio <= 1.0 then
		uiAnim:Flipper( ctrlA, nStartX, nSizeWidth, 0.0, false)
		uiAnim:Flipper( ctrlB, nStartX, nSizeWidth, fRatio*2-1.0, false)
	end
end

function uiAnim:MoveTo( ctrl, nStartX, nStartY, nEndX, nEndY, fCurTime, fDuration)
	fCurTime = uiAnim:Clamp( fCurTime, 0.0, fDuration)

	EaseValue = fDuration * 0.35
	Travel = fDuration * 0.3
	fRatio = uiAnim:EaseInOut( EaseValue, Travel, EaseValue, 1.0, fCurTime)
	
	nLengthX = nEndX - nStartX
	nLengthY = nEndY - nStartY

	ctrl:setPosition( nStartX + nLengthX * fRatio, nStartY + nLengthY * fRatio, 0.0)
end

function uiAnim:FadeOut( ctrl, fCurTime, fDuration)
	local fVal = fCurTime / fDuration
	uiAnim:SetAlpha( ctrl, 1.0 - fVal)
end

function uiAnim:FadeIn( ctrl, fCurTime, fDuration)
	local fVal = fCurTime / fDuration	
	uiAnim:SetAlpha( ctrl, fVal)
end

function uiAnim:PopIn( ctrl, nStartX, nStartY, nSizeX, nSizeY, fScale, fCurTime, fDuration)
	fCurTime = uiAnim:Clamp( fCurTime, 0.0, fDuration)
	
	local EaseValue = fDuration * 0.45
	local Travel = fDuration * 0.1
	local fRatio = uiAnim:EaseInOut( EaseValue, Travel, EaseValue, 1.0, fCurTime)
	
	local fSizeRatio = (1.0 - fRatio) * (fScale - 1.0)
	
	ctrl:setSize( nSizeX + (nSizeX * fSizeRatio), nSizeY + (nSizeY * fSizeRatio))
	
	local nX = (nSizeX*0.5) * (fScale-1.0)
	local nY = (nSizeY*0.5) * (fScale-1.0)
	ctrl:setPosition( nStartX - nX * (1.0-fRatio), nStartY - nY * (1.0-fRatio), 0)
end

function uiAnim:PopOut( ctrl, nStartX, nStartY, nSizeX, nSizeY, fCurTime, fDuration)
	uiAnim:PopIn( ctrl, nStartX, nStartY, nSizeX, nSizeY, 0.0, fCurTime, fDuration)
end