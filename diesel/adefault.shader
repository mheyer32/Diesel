gfx/2d/bigchars
{
	nopicmip
	nomipmaps
	{
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		map bigchars.tga
		rgbgen vertex
	}
}

console
{
	{
		map default.jpg
		tcmod scale 0 0
		rgbgen const 0.5 0 0
	}
}