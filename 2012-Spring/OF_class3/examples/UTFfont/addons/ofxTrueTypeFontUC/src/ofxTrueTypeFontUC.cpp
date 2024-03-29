#include "ofxTrueTypeFontUC.h"
//--------------------------

#include "ft2build.h"
#include "freetype2/freetype/freetype.h"
#include "freetype2/freetype/ftglyph.h"
#include "freetype2/freetype/ftoutln.h"
#include "freetype2/freetype/fttrigon.h"

#include <algorithm>

#include "ofUtils.h"
#include "ofGraphics.h"

static bool printVectorInfo = false;
static int ttfGlobalDpi = 96;

//--------------------------------------------------------

namespace util {
  namespace ofxTrueTypeFontUC {
    
template <class T>
wstring convToUCS4(basic_string<T> src) {
  wstring dst = L"";
  
  // convert UTF-8 on char or wchar_t to UCS-4 on wchar_t
  int size = src.size();
  int index = 0;
  while (index < size) {
    wchar_t c = (unsigned char)src[index];
    if (c < 0x80) {
      dst += (c);
    } else if (c < 0xe0) {
      if (index + 1 < size) {
        dst += (((c & 0x1f) << 6) | (src[index+1] & 0x3f));
        index++;
      }
    } else if (c < 0xf0) {
      if (index + 2 < size) {
        dst += (((c & 0x0f) << 12) | ((src[index+1] & 0x3f) << 6) |
                (src[index+2] & 0x3f));
        index += 2;
      }
    } else if (c < 0xf8) {
      if (index + 3 < size) {
        dst += (((c & 0x07) << 18) | ((src[index+1] & 0x3f) << 12) |
                ((src[index+2] & 0x3f) << 6) | (src[index+3] & 0x3f));
        index += 3;
      }
    } else if (c < 0xfc) {
      if (index + 4 < size) {
        dst += (((c & 0x03) << 24) | ((src[index+1] & 0x3f) << 18) |
                ((src[index+2] & 0x3f) << 12) | ((src[index+3] & 0x3f) << 6) |
                (src[index+4] & 0x3f));
        index += 4;
      }
    } else if (c < 0xfe) {
      if (index + 5 < size) {
        dst += (((c & 0x01) << 30) | ((src[index+1] & 0x3f) << 24) |
                ((src[index+2] & 0x3f) << 18) | ((src[index+3] & 0x3f) << 12) |
                ((src[index+4] & 0x3f) << 6) | (src[index+5] & 0x3f));
        index += 5;
      }
    }
    index++;
  }
  
  return dst;
}

    
wstring convToWString(string src) {
  
#ifdef TARGET_WIN32

  wstring dst = L"";
  typedef codecvt<wchar_t, char, mbstate_t> codecvt_t;
  
  locale loc = locale("");
  if(!std::has_facet<codecvt_t>(loc))
    return dst;
  
  const codecvt_t& conv = use_facet<codecvt_t>(loc);
  
  const std::size_t size = src.length();  
  std::vector<wchar_t> dst_vctr(size);
  wchar_t* const buf = &dst_vctr[0];
  
  const char* dummy;
  wchar_t* next;
  mbstate_t state = {0};
  const char* const s = src.c_str();
  
  if (conv.in(state, s, s + size, dummy, buf, buf + size, next) == codecvt_t::ok) {
    dst = std::wstring(buf, next - buf);
  }
  
  return dst;

#elif defined __clang__
  
  wstring dst = L"";
  for (int i=0; i<src.size(); ++i) {
    dst += src[i];
  }
  
  return dst;
  
#else

  return convToUCS4<char>(src);
  
#endif

}

  } // ofxTrueTypeFontUC
} // util

//--------------------------------------------------------
void ofxTrueTypeFontUC::setGlobalDpi(int newDpi){
	ttfGlobalDpi = newDpi;
}

//--------------------------------------------------------
static ofTTFCharacterUC makeContoursForCharacter(FT_Face &face);
static ofTTFCharacterUC makeContoursForCharacter(FT_Face &face){
	
	//int num			= face->glyph->outline.n_points;
	int nContours	= face->glyph->outline.n_contours;
	int startPos	= 0;
	
	char * tags		= face->glyph->outline.tags;
	FT_Vector * vec = face->glyph->outline.points;
	
	ofTTFCharacterUC charOutlines;
	charOutlines.setUseShapeColor(false);
	
	for(int k = 0; k < nContours; k++){
		if( k > 0 ){
			startPos = face->glyph->outline.contours[k-1]+1;
		}
		int endPos = face->glyph->outline.contours[k]+1;
		
		if( printVectorInfo )printf("--NEW CONTOUR\n\n");
		
		//vector <ofPoint> testOutline;
		ofPoint lastPoint;
		
		for(int j = startPos; j < endPos; j++){
			
			if( FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_ON ){
				lastPoint.set((float)vec[j].x, (float)-vec[j].y, 0);
				if( printVectorInfo )printf("flag[%i] is set to 1 - regular point - %f %f \n", j, lastPoint.x, lastPoint.y);
				//testOutline.push_back(lastPoint);
				charOutlines.lineTo(lastPoint/64);
				
			}else{
				if( printVectorInfo )printf("flag[%i] is set to 0 - control point \n", j);
				
				if( FT_CURVE_TAG(tags[j]) == FT_CURVE_TAG_CUBIC ){
					if( printVectorInfo )printf("- bit 2 is set to 2 - CUBIC\n");
					
					int prevPoint = j-1;
					if( j == 0){
						prevPoint = endPos-1;
					}
					
					int nextIndex = j+1;
					if( nextIndex >= endPos){
						nextIndex = startPos;
					}
					
					ofPoint nextPoint( (float)vec[nextIndex].x,  -(float)vec[nextIndex].y );
					
					//we need two control points to draw a cubic bezier
					bool lastPointCubic =  ( FT_CURVE_TAG(tags[prevPoint]) != FT_CURVE_TAG_ON ) && ( FT_CURVE_TAG(tags[prevPoint]) == FT_CURVE_TAG_CUBIC);
					
					if( lastPointCubic ){
						ofPoint controlPoint1((float)vec[prevPoint].x,	(float)-vec[prevPoint].y);
						ofPoint controlPoint2((float)vec[j].x, (float)-vec[j].y);
						ofPoint nextPoint((float) vec[nextIndex].x,	-(float) vec[nextIndex].y);
						
						//cubic_bezier(testOutline, lastPoint.x, lastPoint.y, controlPoint1.x, controlPoint1.y, controlPoint2.x, controlPoint2.y, nextPoint.x, nextPoint.y, 8);
						charOutlines.bezierTo(controlPoint1.x/64, controlPoint1.y/64, controlPoint2.x/64, controlPoint2.y/64, nextPoint.x/64, nextPoint.y/64);
					}
					
				}else{
					
					ofPoint conicPoint( (float)vec[j].x,  -(float)vec[j].y );
					
					if( printVectorInfo )printf("- bit 2 is set to 0 - conic- \n");
					if( printVectorInfo )printf("--- conicPoint point is %f %f \n", conicPoint.x, conicPoint.y);
					
					//If the first point is connic and the last point is connic then we need to create a virutal point which acts as a wrap around
					if( j == startPos ){
						bool prevIsConnic = (  FT_CURVE_TAG( tags[endPos-1] ) != FT_CURVE_TAG_ON ) && ( FT_CURVE_TAG( tags[endPos-1]) != FT_CURVE_TAG_CUBIC );
						
						if( prevIsConnic ){
							ofPoint lastConnic((float)vec[endPos - 1].x, (float)-vec[endPos - 1].y);
							lastPoint = (conicPoint + lastConnic) / 2;
							
							if( printVectorInfo )	printf("NEED TO MIX WITH LAST\n");
							if( printVectorInfo )printf("last is %f %f \n", lastPoint.x, lastPoint.y);
						}
					}
					
					//bool doubleConic = false;
					
					int nextIndex = j+1;
					if( nextIndex >= endPos){
						nextIndex = startPos;
					}
					
					ofPoint nextPoint( (float)vec[nextIndex].x,  -(float)vec[nextIndex].y );
					
					if( printVectorInfo )printf("--- last point is %f %f \n", lastPoint.x, lastPoint.y);
					
					bool nextIsConnic = (  FT_CURVE_TAG( tags[nextIndex] ) != FT_CURVE_TAG_ON ) && ( FT_CURVE_TAG( tags[nextIndex]) != FT_CURVE_TAG_CUBIC );
					
					//create a 'virtual on point' if we have two connic points
					if( nextIsConnic ){
						nextPoint = (conicPoint + nextPoint) / 2;
						if( printVectorInfo )printf("|_______ double connic!\n");
					}
					if( printVectorInfo )printf("--- next point is %f %f \n", nextPoint.x, nextPoint.y);
					
					//quad_bezier(testOutline, lastPoint.x, lastPoint.y, conicPoint.x, conicPoint.y, nextPoint.x, nextPoint.y, 8);
					charOutlines.quadBezierTo(lastPoint.x/64, lastPoint.y/64, conicPoint.x/64, conicPoint.y/64, nextPoint.x/64, nextPoint.y/64);
					
					if( nextIsConnic ){
						lastPoint = nextPoint;
					}
				}
			}
			
			//end for
		}
		charOutlines.close();
	}
	
	return charOutlines;
}

#ifdef TARGET_ANDROID
#include <set>
set<ofxTrueTypeFontUC*> all_fonts;
void ofUnloadAllFontTextures(){
	set<ofxTrueTypeFontUC*>::iterator it;
	for(it=all_fonts.begin();it!=all_fonts.end();it++){
		(*it)->unloadTextures();
	}
}
void ofReloadAllFontTextures(){
	set<ofxTrueTypeFontUC*>::iterator it;
	for(it=all_fonts.begin();it!=all_fonts.end();it++){
		(*it)->reloadTextures();
	}
}

#endif

bool compare_cps(const charPropsUC & c1, const charPropsUC & c2){
	if(c1.tH == c2.tH) return c1.tW > c2.tW;
	else return c1.tH > c2.tH;
}

//------------------------------------------------------------------
ofxTrueTypeFontUC::ofxTrueTypeFontUC(){
	bLoadedOk		= false;
	bMakeContours	= false;
#ifdef TARGET_ANDROID
	all_fonts.insert(this);
#endif
	//cps				= NULL;
	letterSpacing = 1;
	spaceSize = 1;
	
	// 3 pixel border around the glyph
	// We show 2 pixels of this, so that blending looks good.
	// 1 pixels is hidden because we don't want to see the real edge of the texture
	
	border			= 3;
	//visibleBorder	= 2;
	stringQuads.setMode(OF_PRIMITIVE_TRIANGLES);
	binded = false;
}

//------------------------------------------------------------------
ofxTrueTypeFontUC::~ofxTrueTypeFontUC(){
	
	if (bLoadedOk){
		unloadTextures();
	}
	
#ifdef TARGET_ANDROID
	all_fonts.erase(this);
#endif
}

void ofxTrueTypeFontUC::unloadTextures(){
	if(!bLoadedOk)
		return;
	
	cps.clear();
	textures.clear();
	loadedChars.clear();
	
	// ------------- close the library and typeface
	FT_Done_Face(face);
	FT_Done_FreeType(library);
	
	bLoadedOk = false;
}

void ofxTrueTypeFontUC::reloadTextures(){
	loadFont(filename, fontSize, bAntiAliased, bMakeContours);
}

//===========================================================
const bool ofxTrueTypeFontUC::bFullCharacterSet = true;

//-----------------------------------------------------------
bool ofxTrueTypeFontUC::loadFont(string filename, int fontsize, bool _bAntiAliased, bool makeContours, float _simplifyAmt, int dpi) {
	
	bMakeContours = makeContours;
	
	//------------------------------------------------
	if (bLoadedOk == true){
		// we've already been loaded, try to clean up :
		unloadTextures();
	}
	//------------------------------------------------
	
	if( dpi == 0 ){
		dpi = ttfGlobalDpi;
	}
	
	filename = ofToDataPath(filename);
	
	bLoadedOk 			= false;
	bAntiAliased 		= _bAntiAliased;
	fontSize			= fontsize;
	simplifyAmt = _simplifyAmt;
	
	//--------------- load the library and typeface
	
	FT_Error err = FT_Init_FreeType( &library );
	if (err) {
		ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::loadFont - Error initializing freetype lib: FT_Error = %d", err);
		return false;
	}
	err = FT_New_Face( library, filename.c_str(), 0, &face );
	if (err) {
		// simple error table in lieu of full table (see fterrors.h)
		string errorString = "unknown freetype";
		if(err == 1) errorString = "INVALID FILENAME";
		ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::loadFont - %s: %s: FT_Error = %d", errorString.c_str(), filename.c_str(), err);
		return false;
	}
	
	FT_Set_Char_Size( face, fontSize << 6, fontSize << 6, dpi, dpi);
	lineHeight = fontSize * 1.43f;
	
	//------------------------------------------------------
	//kerning would be great to support:
	//ofLog(OF_LOG_NOTICE,"FT_HAS_KERNING ? %i", FT_HAS_KERNING(face));
	//------------------------------------------------------
	
	setNumMaxCharacters(DEFAULT_NUM_MAX_CHARACTERS);
	
	bLoadedOk = true;
	return true;
}

//-----------------------------------------------------------
bool ofxTrueTypeFontUC::isLoaded() {
	return bLoadedOk;
}

//-----------------------------------------------------------
bool ofxTrueTypeFontUC::isAntiAliased() {
	return bAntiAliased;
}

//-----------------------------------------------------------
bool ofxTrueTypeFontUC::hasFullCharacterSet() {
	return bFullCharacterSet;
}

//-----------------------------------------------------------
int ofxTrueTypeFontUC::getSize() {
	return fontSize;
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::setLineHeight(float _newLineHeight) {
	lineHeight = _newLineHeight;
}

//-----------------------------------------------------------
float ofxTrueTypeFontUC::getLineHeight(){
	return lineHeight;
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::setLetterSpacing(float _newletterSpacing) {
	letterSpacing = _newletterSpacing;
}

//-----------------------------------------------------------
float ofxTrueTypeFontUC::getLetterSpacing(){
	return letterSpacing;
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::setSpaceSize(float _newspaceSize) {
	spaceSize = _newspaceSize;
}

//-----------------------------------------------------------
float ofxTrueTypeFontUC::getSpaceSize(){
	return spaceSize;
}

//------------------------------------------------------------------
ofTTFCharacterUC ofxTrueTypeFontUC::getCharacterAsPoints(wstring character) {
  
  wstring c = util::ofxTrueTypeFontUC::convToUCS4<wchar_t>(character);
  
  int charID = getCharID(c[0]);
  if(cps[charID].character == TYPEFACE_UNLOADED){
    loadChar(charID);
  }
  return getCharacterAsPointsFromCharID(charID);
}

ofTTFCharacterUC ofxTrueTypeFontUC::getCharacterAsPoints(string character) {
  
  return getCharacterAsPoints(util::ofxTrueTypeFontUC::convToWString(character));
}

ofTTFCharacterUC ofxTrueTypeFontUC::getCharacterAsPointsFromCharID(const int &charID) {
	if( bMakeContours == false ){
		ofLog(OF_LOG_ERROR, "getCharacterAsPoints: contours not created,  call loadFont with makeContours set to true" );
	}
	
	if( bMakeContours && (int)charOutlines.size() > 0) {
		return charOutlines[charID];
	}else{
		if(charOutlines.empty())
			charOutlines.push_back(ofTTFCharacterUC());
		return charOutlines[0];
	}
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::drawChar(int c, float x, float y) {
	
	if (c >= nMaxCharacters){
		//ofLog(OF_LOG_ERROR,"Error : char (%i) not allocated -- line %d in %s", (c + NUM_CHARACTER_TO_START), __LINE__,__FILE__);
		return;
	}
	
	GLfloat	x1, y1, x2, y2;
	GLfloat t1, v1, t2, v2;
	t2		= cps[c].t2;
	v2		= cps[c].v2;
	t1		= cps[c].t1;
	v1		= cps[c].v1;
	
	x1		= cps[c].x1+x;
	y1		= cps[c].y1+y;
	x2		= cps[c].x2+x;
	y2		= cps[c].y2+y;
	
	int firstIndex = stringQuads.getVertices().size();
	
	stringQuads.addVertex(ofVec3f(x1,y1));
	stringQuads.addVertex(ofVec3f(x2,y1));
	stringQuads.addVertex(ofVec3f(x2,y2));
	stringQuads.addVertex(ofVec3f(x1,y2));
	
	stringQuads.addTexCoord(ofVec2f(t1,v1));
	stringQuads.addTexCoord(ofVec2f(t2,v1));
	stringQuads.addTexCoord(ofVec2f(t2,v2));
	stringQuads.addTexCoord(ofVec2f(t1,v2));
	
	stringQuads.addIndex(firstIndex);
	stringQuads.addIndex(firstIndex+1);
	stringQuads.addIndex(firstIndex+2);
	stringQuads.addIndex(firstIndex+2);
	stringQuads.addIndex(firstIndex+3);
	stringQuads.addIndex(firstIndex);
}

//-----------------------------------------------------------
vector<ofTTFCharacterUC> ofxTrueTypeFontUC::getStringAsPoints(wstring s){
  
#ifdef __llvm__
#ifdef __clang__
  s = util::ofxTrueTypeFontUC::convToUCS4<wchar_t>(s);
#endif
#endif

	vector<ofTTFCharacterUC> shapes;
	
	if (!bLoadedOk){
		ofLog(OF_LOG_ERROR,"Error : font not allocated -- line %d in %s", __LINE__,__FILE__);
		return shapes;
	};
	
	GLint		index	= 0;
	GLfloat		X		= 0;
	GLfloat		Y		= 0;
	
	int len = (int)s.length();
	
	while(index < len){
		int cy = getCharID(s[index]);
		if(cps[cy].character == TYPEFACE_UNLOADED){
			loadChar(cy);
		}
		if (cy < nMaxCharacters){ 			// full char set or not?
			if (s[index] == L'\n') {
				Y += lineHeight;
				X = 0 ; //reset X Pos back to zero
			}
			else if (s[index] == L' ') {
				// int cy = (int)'p' - NUM_CHARACTER_TO_START;
				int cy = getCharID(L'p');
				X += cps[cy].width * letterSpacing * spaceSize;
			}
			else {
				shapes.push_back(getCharacterAsPointsFromCharID(cy));
				shapes.back().translate(ofPoint(X,Y));
				X += cps[cy].setWidth * letterSpacing;
			}
		}
		index++;
	}
	return shapes;
	
}

vector<ofTTFCharacterUC> ofxTrueTypeFontUC::getStringAsPoints(string s) {

  return getStringAsPoints(util::ofxTrueTypeFontUC::convToWString(s));
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::drawCharAsShape(int c, float x, float y) {
	if (c >= nMaxCharacters){
		//ofLog(OF_LOG_ERROR,"Error : char (%i) not allocated -- line %d in %s", (c + NUM_CHARACTER_TO_START), __LINE__,__FILE__);
		return;
	}
	//-----------------------
	
	int cu = c;
	ofTTFCharacterUC & charRef = charOutlines[cu];
	charRef.setFilled(ofGetStyle().bFill);
	charRef.draw(x,y);
}

//-----------------------------------------------------------
float ofxTrueTypeFontUC::stringWidth(wstring c) {

#ifdef __llvm__
#ifdef __clang__
  c = util::ofxTrueTypeFontUC::convToUCS4<wchar_t>(c);
#endif
#endif

	ofRectangle rect = getStringBoundingBox(c, 0,0);
	return rect.width;
}

float ofxTrueTypeFontUC::stringWidth(string s) {
  
  return stringWidth(util::ofxTrueTypeFontUC::convToWString(s));
}


ofRectangle ofxTrueTypeFontUC::getStringBoundingBox(wstring c, float x, float y){

#ifdef __llvm__
#ifdef __clang__
  c = util::ofxTrueTypeFontUC::convToUCS4<wchar_t>(c);
#endif
#endif

	ofRectangle myRect;
	
	if (!bLoadedOk){
		ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::getStringBoundingBox - font not allocated");
		return myRect;
	}
	
	GLint		index	= 0;
	GLfloat		xoffset	= 0;
	GLfloat		yoffset	= 0;
	int         len     = (int)c.length();
	float       minx    = -1;
	float       miny    = -1;
	float       maxx    = -1;
	float       maxy    = -1;
	
	if ( len < 1 || cps.empty() ){
		myRect.x        = 0;
		myRect.y        = 0;
		myRect.width    = 0;
		myRect.height   = 0;
		return myRect;
	}
	
	bool bFirstCharacter = true;
	while(index < len){
		// int cy = (unsigned char)c[index] - NUM_CHARACTER_TO_START;
		int cy = getCharID(c[index]);
		if(cps[cy].character == TYPEFACE_UNLOADED){
			loadChar(cy);
		}
		if (cy < nMaxCharacters){ 			// full char set or not?
			if (c[index] == L'\n') {
				yoffset += lineHeight;
				xoffset = 0 ; //reset X Pos back to zero
			}
			else if (c[index] == L' ') {
				// int cy = (int)'p' - NUM_CHARACTER_TO_START;
				int cy = getCharID(L'p');
				xoffset += cps[cy].width * letterSpacing * spaceSize;
				// zach - this is a bug to fix -- for now, we don't currently deal with ' ' in calculating string bounding box
			}
			else {
				GLint height	= cps[cy].height;
				GLint bwidth	= cps[cy].width * letterSpacing;
				GLint top		= cps[cy].topExtent - cps[cy].height;
				GLint lextent	= cps[cy].leftExtent;
				float	x1, y1, x2, y2, corr, stretch;
				stretch = 0;//(float)visibleBorder * 2;
				corr = (float)(((fontSize - height) + top) - fontSize);
				x1		= (x + xoffset + lextent + bwidth + stretch);
				y1		= (y + yoffset + height + corr + stretch);
				x2		= (x + xoffset + lextent);
				y2		= (y + yoffset + -top + corr);
				xoffset += cps[cy].setWidth * letterSpacing;
				if (bFirstCharacter == true){
					minx = x2;
					miny = y2;
					maxx = x1;
					maxy = y1;
					bFirstCharacter = false;
				} else {
					if (x2 < minx) minx = x2;
					if (y2 < miny) miny = y2;
					if (x1 > maxx) maxx = x1;
					if (y1 > maxy) maxy = y1;
				}
			}
		}
		index++;
	}
	
	myRect.x        = minx;
	myRect.y        = miny;
	myRect.width    = maxx-minx;
	myRect.height   = maxy-miny;
	return myRect;
}

ofRectangle ofxTrueTypeFontUC::getStringBoundingBox(string s, float x, float y){

  return getStringBoundingBox(util::ofxTrueTypeFontUC::convToWString(s), x, y);
}

//-----------------------------------------------------------
float ofxTrueTypeFontUC::stringHeight(wstring c) {

#ifdef __llvm__
#ifdef __clang__
  c = util::ofxTrueTypeFontUC::convToUCS4<wchar_t>(c);
#endif
#endif

	ofRectangle rect = getStringBoundingBox(c, 0,0);
	return rect.height;
}

float ofxTrueTypeFontUC::stringHeight(string s) {
  
  return stringHeight(util::ofxTrueTypeFontUC::convToWString(s));
}

//=====================================================================
void ofxTrueTypeFontUC::drawString(wstring c, float x, float y) {

#ifdef __llvm__
#ifdef __clang__
  c = util::ofxTrueTypeFontUC::convToUCS4<wchar_t>(c);
#endif
#endif

	/*glEnable(GL_BLEND);
	 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	 texAtlas.draw(0,0);*/
	
	if (!bLoadedOk){
		ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::drawString - Error : font not allocated -- line %d in %s", __LINE__,__FILE__);
		return;
	};
	
	GLint		index	= 0;
	GLfloat		X		= x;
	GLfloat		Y		= y;
	
	
	bool alreadyBinded = binded;
	
	int len = (int)c.length();
	
	while(index < len){
		int cy = getCharID(c[index]);
		if(cps[cy].character == TYPEFACE_UNLOADED){
			loadChar(cy);
		}
		
		if (cy < nMaxCharacters){ 			// full char set or not?
			if (c[index] == L'\n') {
				Y += lineHeight;
				X = x ; //reset X Pos back to zero
			}
			else if (c[index] == L' ') {
				int cy = getCharID(L'p');
				X += cps[cy].width * letterSpacing * spaceSize;
			}
			else {
				bind(cy);
				drawChar(cy, X, Y);
				unbind(cy);
				X += cps[cy].setWidth * letterSpacing;
			}
		}
		
		index++;
	}  
}

void ofxTrueTypeFontUC::drawString(string s, float x, float y) {
 
  return drawString(util::ofxTrueTypeFontUC::convToWString(s), x, y);
}

//=====================================================================
void ofxTrueTypeFontUC::drawStringAsShapes(wstring c, float x, float y) {
	
	if (!bLoadedOk){
		ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::drawStringAsShapes - Error : font not allocated -- line %d in %s", __LINE__,__FILE__);
		return;
	};
	
	//----------------------- error checking
	if (!bMakeContours){
		ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::drawStringAsShapes - Error : contours not created for this font - call loadFont with makeContours set to true");
		return;
	}
  
#ifdef __llvm__
#ifdef __clang__
  c = util::ofxTrueTypeFontUC::convToUCS4<wchar_t>(c);
#endif
#endif

	
	GLint		index	= 0;
	GLfloat		X		= 0;
	GLfloat		Y		= 0;
	
	ofPushMatrix();
	ofTranslate(x, y);
	int len = (int)c.length();
	
	while(index < len){
		// int cy = (unsigned char)c[index] - NUM_CHARACTER_TO_START;
		int cy = getCharID(c[index]);
		if(cps[cy].character == TYPEFACE_UNLOADED){
			loadChar(cy);
		}
		if (cy < nMaxCharacters){ 			// full char set or not?
			if (c[index] == L'\n') {
				Y += lineHeight;
				X = 0 ; //reset X Pos back to zero
			}
			else if (c[index] == L' ') {
				// int cy = (int)'p' - NUM_CHARACTER_TO_START;
				int cy = getCharID(L'p');
				X += cps[cy].width;
				//glTranslated(cps[cy].width, 0, 0);
			} else {
				drawCharAsShape(cy, X, Y);
				X += cps[cy].setWidth;
				//glTranslated(cps[cy].setWidth, 0, 0);
			}
		}
		index++;
	}
	
	ofPopMatrix();
	
}

void ofxTrueTypeFontUC::drawStringAsShapes(string s, float x, float y) {
  
  return drawStringAsShapes(util::ofxTrueTypeFontUC::convToWString(s), x, y);
}

//-----------------------------------------------------------
int ofxTrueTypeFontUC::getNumCharacters() {
	// return nCharacters;
	return loadedChars.size();
}

//=====================================================================
const int ofxTrueTypeFontUC::TYPEFACE_UNLOADED = 0;
const int ofxTrueTypeFontUC::DEFAULT_NUM_MAX_CHARACTERS = 100000;

//-----------------------------------------------------------
void ofxTrueTypeFontUC::bind(const int &charID){
	if(!binded){
		// we need transparency to draw text, but we don't know
		// if that is set up in outside of this function
		// we "pushAttrib", turn on alpha and "popAttrib"
		// http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/pushattrib.html
		
		// **** note ****
		// I have read that pushAttrib() is slow, if used often,
		// maybe there is a faster way to do this?
		// ie, check if blending is enabled, etc...
		// glIsEnabled().... glGet()...
		// http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/get.html
		// **************
		// (a) record the current "alpha state, blend func, etc"
#ifndef TARGET_OPENGLES
		glPushAttrib(GL_COLOR_BUFFER_BIT);
#else
		blend_enabled = glIsEnabled(GL_BLEND);
		texture_2d_enabled = glIsEnabled(GL_TEXTURE_2D);
		glGetIntegerv( GL_BLEND_SRC, &blend_src );
		glGetIntegerv( GL_BLEND_DST, &blend_dst );
#endif
		
		// (b) enable our regular ALPHA blending!
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		textures[charID].bind();
		stringQuads.clear();
		binded = true;
	}
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::unbind(const int &charID){
	if(binded){
		stringQuads.drawFaces();
		textures[charID].unbind();
		
#ifndef TARGET_OPENGLES
		glPopAttrib();
#else
		if( !blend_enabled )
			glDisable(GL_BLEND);
		if( !texture_2d_enabled )
			glDisable(GL_TEXTURE_2D);
		glBlendFunc( blend_src, blend_dst );
#endif
		binded = false;
	}
}

//-----------------------------------------------------------
int ofxTrueTypeFontUC::getNumMaxCharacters()
{
	return nMaxCharacters;
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::setNumMaxCharacters(int num)
{
	if(num<=0)
		return;
	
	nMaxCharacters = num;
	
	vector<charPropsUC>().swap(cps);
	vector<ofTexture>().swap(textures);
	vector<int>().swap(loadedChars);
	vector<ofTTFCharacterUC>().swap(charOutlines);
	
	//--------------- initialize character info and textures
	cps.resize(nMaxCharacters);
	for(int i=0; i<nMaxCharacters; ++i){
		cps[i].character = TYPEFACE_UNLOADED;
	}
	textures.resize(nMaxCharacters);
	
	if(bMakeContours){
		charOutlines.clear();
		charOutlines.assign(nMaxCharacters, ofTTFCharacterUC());
	}
	
	//--------------- load 'p' character for display ' '
	int cy = getCharID(L'p');
	loadChar(cy);
}

//-----------------------------------------------------------
int ofxTrueTypeFontUC::getCharID(const int &c) {
	int tmp = (int)c - NUM_CHARACTER_TO_START_UC;
	int point = 0;
	for (; point != (int)loadedChars.size(); ++point) {
		if (loadedChars[point] == tmp) {
			break;
		}
	}
	if (point == loadedChars.size()) {
		//----------------------- error checking
		if (point >= nMaxCharacters){
			ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::getCharID - Error : too many typeface already loaded - call loadFont to reset");
			return point = 0;
		}
		else {
			loadedChars.push_back(tmp);
		}
	}
	return point;
}

//-----------------------------------------------------------
void ofxTrueTypeFontUC::loadChar(const int &charID) {
	int i = charID;
	ofPixels expandedData;
	
	//------------------------------------------ anti aliased or not:
	FT_Error err = FT_Load_Glyph( face, FT_Get_Char_Index( face, loadedChars[i]+NUM_CHARACTER_TO_START_UC ), FT_LOAD_DEFAULT );
	if(err){
		ofLog(OF_LOG_ERROR,"ofxTrueTypeFontUC::loadFont - Error with FT_Load_Glyph %i: FT_Error = %d", loadedChars[i]+NUM_CHARACTER_TO_START_UC, err);
	}
	
	if (bAntiAliased == true)
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	else
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);
	
	//------------------------------------------
	FT_Bitmap& bitmap= face->glyph->bitmap;
	
	
	// prepare the texture:
	/*int width  = ofNextPow2( bitmap.width + border*2 );
	 int height = ofNextPow2( bitmap.rows  + border*2 );
	 
	 
	 // ------------------------- this is fixing a bug with small type
	 // ------------------------- appearantly, opengl has trouble with
	 // ------------------------- width or height textures of 1, so we
	 // ------------------------- we just set it to 2...
	 if (width == 1) width = 2;
	 if (height == 1) height = 2;*/
	
	
	if(bMakeContours){
		if( printVectorInfo )
			printf("\n\ncharacter charID %d: \n", i );
		
		//int character = i + NUM_CHARACTER_TO_START;
		charOutlines[i] = makeContoursForCharacter( face );
		if(simplifyAmt>0)
			charOutlines[i].simplify(simplifyAmt);
		charOutlines[i].getTessellation();
	}
	
	
	// -------------------------
	// info about the character:
	cps[i].character		= loadedChars[i];
	cps[i].height 			= face->glyph->bitmap_top;
	cps[i].width 			= face->glyph->bitmap.width;
	cps[i].setWidth 		= face->glyph->advance.x >> 6;
	cps[i].topExtent 		= face->glyph->bitmap.rows;
	cps[i].leftExtent		= face->glyph->bitmap_left;
	
	int width  = cps[i].width;
	int height = bitmap.rows;
	
	cps[i].tW = width;
	cps[i].tH = height;
	
	GLint fheight	= cps[i].height;
	GLint bwidth	= cps[i].width;
	GLint top		= cps[i].topExtent - cps[i].height;
	GLint lextent	= cps[i].leftExtent;
	
	GLfloat	corr, stretch;
	
	//this accounts for the fact that we are showing 2*visibleBorder extra pixels
	//so we make the size of each char that many pixels bigger
	stretch = 0;//(float)(visibleBorder * 2);
	
	corr	= (float)(( (fontSize - fheight) + top) - fontSize);
	
	cps[i].x1		= lextent + bwidth + stretch;
	cps[i].y1		= fheight + corr + stretch;
	cps[i].x2		= (float) lextent;
	cps[i].y2		= -top + corr;
	
	// Allocate Memory For The Texture Data.
	expandedData.allocate(width, height, 2);
	//-------------------------------- clear data:
	expandedData.set(0,255); // every luminance pixel = 255
	expandedData.set(1,0);
	
	if (bAntiAliased == true){
		ofPixels bitmapPixels;
		bitmapPixels.setFromExternalPixels(bitmap.buffer,bitmap.width,bitmap.rows,1);
		expandedData.setChannel(1,bitmapPixels);
	} else {
		//-----------------------------------
		// true type packs monochrome info in a
		// 1-bit format, hella funky
		// here we unpack it:
		unsigned char *src =  bitmap.buffer;
		for(int j=0; j <bitmap.rows;j++) {
			unsigned char b=0;
			unsigned char *bptr =  src;
			for(int k=0; k < bitmap.width ; k++){
				expandedData[2*(k+j*width)] = 255;
				
				if (k%8==0){
					b = (*bptr++);
				}
				
				expandedData[2*(k+j*width) + 1] = b&0x80 ? 255 : 0;
				b <<= 1;
			}
			src += bitmap.pitch;
		}
		//-----------------------------------
	}
	
	int longSide = border*2;
	cps[i].tW > cps[i].tH ? longSide += cps[i].tW : longSide += cps[i].tH;
    
	int tmp = 1;
	while(longSide > tmp) {
		tmp <<= 1;
	}
	int w = tmp;
	int h = w;
	
	ofPixels atlasPixels;
	atlasPixels.allocate(w,h,2);
	atlasPixels.set(0,255);
	atlasPixels.set(1,0);
    
	cps[i].t2		= float(border)/float(w);
	cps[i].v2		= float(border)/float(h);
	cps[i].t1		= float(cps[i].tW + border)/float(w);
	cps[i].v1		= float(cps[i].tH + border)/float(h);
	expandedData.pasteInto(atlasPixels, border, border);
	
	textures[i].allocate(atlasPixels.getWidth(),atlasPixels.getHeight(),GL_LUMINANCE_ALPHA,false);
	
	if(bAntiAliased && fontSize>20){
		textures[i].setTextureMinMagFilter(GL_LINEAR,GL_LINEAR);
	}else{
		textures[i].setTextureMinMagFilter(GL_NEAREST,GL_NEAREST);
	}
	
	textures[i].loadData(atlasPixels.getPixels(),atlasPixels.getWidth(),atlasPixels.getHeight(),GL_LUMINANCE_ALPHA);
}




