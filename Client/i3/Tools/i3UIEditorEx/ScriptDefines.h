#pragma once

const int blockSize = 128 * 1024;
const char appName[] = "i3UIEditor";			//"DMApp";
const char className[] = "i3UIEditor Window";	//"DMAppWindow";

// A few basic colors
const COLORREF black = RGB( 0, 0, 0 );
const COLORREF white = RGB( 255, 255, 255 );
const COLORREF green = RGB( 0, 255, 0 );
const COLORREF red = RGB( 255, 0, 0 );
const COLORREF blue = RGB( 0, 0, 255 );
const COLORREF yellow = RGB( 255, 255, 0 );
const COLORREF magenta = RGB( 255, 0, 255 );
const COLORREF cyan = RGB( 0, 255, 255 );

const COLORREF orange = RGB( 248,115, 16);
const COLORREF green2 = RGB(16,167,57);
const COLORREF blue2 = RGB(24,100,226);

struct SScintillaColors
{   int         iItem;
    COLORREF    rgb;
};

static SScintillaColors g_rgbSyntaxCpp[] = 
{
    {   SCE_C_COMMENT,          green2 },
    {   SCE_C_COMMENTLINE,      green2 },
    {   SCE_C_COMMENTDOC,       green2 },
    {   SCE_C_NUMBER,           magenta },
    {   SCE_C_STRING,           orange },
    {   SCE_C_CHARACTER,        orange },
    {   SCE_C_UUID,             cyan },
    {   SCE_C_OPERATOR,         black },
    {   SCE_C_PREPROCESSOR,     blue },
    {   SCE_C_WORD,             blue2 },
    {   -1,                     0 }
};

// Lua Keywords from "C:\Program Files (x86)\Lua\5.1\SciTE\lua.properties"
static const char g_luakeywords[] = 
	"and break do else elseif end false for function if "
	"in local nil not or repeat return then true until while "

	"_VERSION assert collectgarbage dofile error gcinfo loadfile loadstring "
	"print rawget rawset require tonumber tostring type unpack "

	"_ALERT _ERRORMESSAGE _INPUT _PROMPT _OUTPUT "
	"_STDERR _STDIN _STDOUT call dostring foreach foreachi getn globals newtype "
	"sort tinsert tremove "

	"abs acos asin atan atan2 ceil cos deg exp "
	"floor format frexp gsub ldexp log log10 max min mod rad random randomseed "
	"sin sqrt strbyte strchar strfind strlen strlower strrep strsub strupper tan "

	"openfile closefile readfrom writeto appendto "
	"remove rename flush seek tmpfile tmpname read write "
	"clock date difftime execute exit getenv setlocale time "

	"_G getfenv getmetatable ipairs loadlib next pairs pcall "
	"rawequal setfenv setmetatable xpcall "
	"string table math coroutine io os debug "
	"load module select "

	"string.byte string.char string.dump string.find string.len "
	"string.lower string.rep string.sub string.upper string.format string.gfind string.gsub "
	"table.concat table.foreach table.foreachi table.getn table.sort table.insert table.remove table.setn "
	"math.abs math.acos math.asin math.atan math.atan2 math.ceil math.cos math.deg math.exp "
	"math.floor math.frexp math.ldexp math.log math.log10 math.max math.min math.mod "
	"math.pi math.pow math.rad math.random math.randomseed math.sin math.sqrt math.tan "
	"string.gmatch string.match string.reverse table.maxn "
	"math.cosh math.fmod math.modf math.sinh math.tanh math.huge "

	"coroutine.create coroutine.resume coroutine.status "
	"coroutine.wrap coroutine.yield "
	"io.close io.flush io.input io.lines io.open io.output io.read io.tmpfile io.type io.write "
	"io.stdin io.stdout io.stderr "
	"os.clock os.date os.difftime os.execute os.exit os.getenv os.remove os.rename "
	"os.setlocale os.time os.tmpname "
	"coroutine.running package.cpath package.loaded package.loadlib package.path "
	"package.preload package.seeall io.popen "

	"";