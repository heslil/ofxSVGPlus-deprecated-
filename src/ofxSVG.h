#pragma once
/*
 This library was rewritten by Joshua Noble from the ofxSVGTiny library.
 It was meant for one way parsing of an SVG file to native ofPaths. 
 
 I have added the ability to modify the colour, visibility and line width of those paths
 and then save the new SVG out again. No provision is made for modifying the curve shapes,
 but it could be implemented along the same lines.
 
 This makes it possible to modify and save vector graphcis also on iOS.
 
 
 
 TODO: There seems to be an issue with a flickeing blue stroke appearing sometimes. Dunnot what that is.
 
 Andreas Borg
 crea.tion.to
 
 */
#include "ofMain.h"
#include "svgtiny.h"
#include "ofPath.h"
#include "ofTypes.h"






#include "Poco/URIStreamOpener.h"  
#include "Poco/StreamCopier.h"  
#include "Poco/Path.h"  
#include "Poco/URI.h"  
#include "Poco/Exception.h"  
#include "Poco/Net/HTTPStreamFactory.h"  
#include "Poco/XML/XMLString.h"  
#include "Poco/DOM/DOMParser.h"  
#include "Poco/DOM/Document.h"  
#include "Poco/DOM/Attr.h"  
#include "Poco/DOM/NodeIterator.h"  
#include "Poco/DOM/NodeFilter.h"  
#include "Poco/DOM/NamedNodeMap.h"   
#include "Poco/DOM/ChildNodesList.h"
//BORG
#include "Poco/DOM/DOMWriter.h"
//#include "Poco/XML/XMLReader.h" 
#include "Poco/XML/XMLWriter.h" 
#include "Poco/UTF8Encoding.h"
#include "ofxSVGTypes.h"


using namespace Poco::XML;

class ofxSVG {
	public: 
        //borg added constructor. Without it, instance variables won't be inited. Print me a fkn T-shirt!
        ofxSVG();
        //-borg
        ~ofxSVG();


		float getWidth() const {
			return width;
		}
		float getHeight() const {
			return height;
		}
		void load(string path);
		void draw();

        int getNumPath();
        ofPath & getPathAt(int n);        
    
    
        //borg mod
        //applies to whole shape
        void setFilled(bool t);
        void setFillColor(ofColor col);
        void setStrokeWidth(float f);
        void setStrokeColor(ofColor col);//use to set alpha too
        bool getFilled();
        ofColor getFillColor();
        float getStrokeWidth();
        ofColor getStrokeColor();
    
    
        //applies to specific path
        void setFilled(bool t,int path);
        void setFillColor(ofColor col,int path);//use to set alpha too
        void setStrokeWidth(float f,int path);
        void setStrokeColor(ofColor col,int path);//use to set alpha too
        bool getFilled(int path);
        ofColor getFillColor(int path);
        float getStrokeWidth(int path);
        ofColor getStrokeColor(int path);
    
        string hexToWeb(ofColor col){
            return "#"+ofToHex(col.r)+ofToHex(col.g)+ofToHex(col.b);
        }
    
    
    
    //Functions to build new SVGs
   /* Element * xmlGetSVG();//the main svg node
    void xmlSetSVG(Element * xml);
    Element * xmlGetBody();
    void xmlSetBody(Element * xml);

    void xmlSetPath(int i, Element * xml);
    
    
    //utility to create new complex svg
    void xmlBeginGroup();
    void xmlEndGroup();
    int xmlGetGroupNum();
    void xmlAddPathToGroup(int group, Element * xml);
    Element * xmlGetPathFromGroup(int group,int path);
    
    
    
    int xmlGetSVGNum();
    void xmlNestSVG(Element * xml);//you can nest one svg node inside another
    
    //convert a path to xml element
    //static Element * xmlGetPathAsXML(vector<ofPoint>,ofColor fill, ofColor stroke,float strokeWidth);
    */
    
        void merge(ofxSVG & svg);//add to current
        void save(string file);
        string toString();//return the whole svg as a xml document string..have fun

    
    //Poco::XML::Document *svgDocument;
    
        svgInfo info;
        typedef ofPtr <ofPath> ofPathRef;//note this! Shorthand for paths as ofPtrs
        vector <ofPathRef> paths;
        void parseXML(string xml);
        
    //use when creasting new ofxSVG
   
        void setSize(int w, int h, string unit="px");
        void setPos(int x, int y);
        int getX();
        int getY();
        int getWidth();
        int getHeight();
        void setViewbox(int x, int y,int w, int h);

    
	private:
        
        void parseHeader(Document *);
        void xmlCreateSVG(Document *,Element *,ofPtr<svgNode> );
        Element * parseNode(Document *doc,ofPtr<svgNode> );
        void setSVGattribute(ofPtr<svgNode>  node,string attribute, string value);
        bool isFilled;
        ofColor fill;
        ofColor stroke;
        float strokeWidth;

    
        //void updatePath(
    
		float width, height;
        float x,y;


		void setupDiagram(struct svgtiny_diagram * diagram);
		void setupShape(struct svgtiny_shape * shape);

};



typedef ofPtr <ofxSVG> ofxSVGRef;
