#pragma once
/*
 This library was rewritten by Joshua Noble from the ofxSVGTiny library.
 It was meant for one way parsing of an SVG file to native ofPaths. 
 
 I have added the ability to modify the colour, visibility and line width of those paths
 and then save the new SVG out again. No provision is made for modifying the curve shapes,
 but it could be implemented along the same lines.
 
 This makes it possible to modify and save vector graphcis also on iOS.
 
 
 
 
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


//#include "ofxClipper.h"


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
        //reparse is required to update doc after changes, but when adding many
        //paths better to do after all is said and done
        void addPath(ofPath &path,bool inNewGroup=false);
        void addPaths(vector<ofPath> &paths,bool inNewGroup=false);
    
    
        void merge(ofxSVG & svg);//add to current
        void save(string file);
        string toString();//return the whole svg as a xml document string..have fun

    

    
        svgInfo info;
        typedef ofPtr <ofPath> ofPathRef;//note this! Shorthand for paths as ofPtrs
        vector <ofPathRef> paths;
        void parseXML(string xml);
        
    //use when creating new ofxSVG
   
        void setSize(int w, int h, string unit="px");
        void setPos(ofPoint pt);
        void setPos(int x, int y);
        int getX();
        int getY();
        int getWidth();
        int getHeight();
        void setViewbox(int x, int y,int w, int h);
    
    
        //return as ofImage
        ofImage getImage(int MSAA = 1);
        ofImage getImage(int w, int h,ofColor bg, int MSAA = 1);
    
    //ofPath:tesselator crashes when too many commands, test for more than 10000
    void setComplexityThreshold(int i){
        complexityThreshold = i;
    }
    int getComplexityThreshold(){
        return complexityThreshold;
    }
    /*
    //logical operations
    ofPtr <vector <ofPolyline> > logicalIntersection(ofxSVG & svg);
    ofPtr <vector <ofPolyline> > logicalDifference(ofxSVG & svg);
    ofPtr <vector <ofPolyline> > logicalUnion(ofxSVG & svg);
    ofPtr <vector <ofPolyline> > logicalXor(ofxSVG & svg);
    */
    
	private:
    
        int complexityThreshold;//to avoid tesselator crash
    
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
    
    
    Element *parsePath(ofPath path,Document * document);
    /*
    ofxClipper clipper;
    ofxPolylines clipMasks;
    ofxPolylines clipSubjects;
    ofxPolylines clips;
    ofxPolylines offsets;
    void generateClipper(ofxSVG* src,ofxSVG * clip);
*/
};


typedef ofPtr <ofxSVG> ofxSVGRef;
typedef ofPtr <vector <ofPolyline> > ofPolylinesRef;
