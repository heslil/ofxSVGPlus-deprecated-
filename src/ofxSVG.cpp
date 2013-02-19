#include "ofxSVG.h"

//included by borg...
//#include "svgtiny_internal.h"


using namespace std;
using namespace Poco::XML;


ofxSVG::ofxSVG(){
    ofPtr<svgNode> rootnode(new svgNode());
    rootnode->type = SVG_TAG_TYPE_DOCUMENT;
    info.rootnode = rootnode;
    x=0;
    y=0;
   
};
ofxSVG::~ofxSVG(){
	paths.clear();
    info.rootnode.reset();
    for(int i = 0; i < info.flatlist.size(); i++){
        info.flatlist[i].reset();
    }
    info.flatlist.clear();
    //xmlFree(svgDocument);
    
}


int ofxSVG::getNumPath(){
    return paths.size();
}
ofPath & ofxSVG::getPathAt(int n){
    return *paths[n];
}

void ofxSVG::load(string path){
	path = ofToDataPath(path);

	if(path.compare("") == 0){
		stringstream ss;
		ss << path << " does not exist " << endl;
		ofLog(OF_LOG_ERROR, ss.str());
		return;
	}

	ofBuffer buffer = ofBufferFromFile(path);
	//size_t size = buffer.size();

    parseXML(buffer.getText());
    
    
   // ofLog()<<"paths size "<<info.rootnode->type<<" getNumPath "<<getNumPath()<<" " <<paths.size()<<endl;

}


void ofxSVG::parseXML(string xml){
    
    
    if( info.rootnode){
         info.rootnode.reset();
    }
    
    info.flatlist.clear();
    
    ofPtr<svgNode> rootnode(new svgNode());
    rootnode->type = SVG_TAG_TYPE_DOCUMENT;
    info.rootnode = rootnode;
    x=0;
    y=0;
    if(paths.size()>0){
        paths.clear();
    }
    
    
    struct svgtiny_diagram * diagram = svgtiny_create();
    
	svgtiny_code code = svgtiny_parse(info,diagram, xml.c_str(), 0, 0);
    //svgtiny_parse(diagram, buffer.getText().c_str(), size, path.c_str(), 0, 0);
    
    
    
    
    if(code != svgtiny_OK){
		fprintf(stderr, "svgtiny_parse failed: ");
		switch(code){
            case svgtiny_OUT_OF_MEMORY:
                fprintf(stderr, "svgtiny_OUT_OF_MEMORY");
                break;
                
            case svgtiny_LIBXML_ERROR:
                fprintf(stderr, "svgtiny_LIBXML_ERROR");
                break;
                
            case svgtiny_NOT_SVG:
                fprintf(stderr, "svgtiny_NOT_SVG");
                break;
                
            case svgtiny_SVG_ERROR:
                fprintf(stderr, "svgtiny_SVG_ERROR: line %i: %s",
                        diagram->error_line,
                        diagram->error_message);
                break;
                
            default:
                fprintf(stderr, "unknown svgtiny_code %i", code);
                break;
		}
		fprintf(stderr, "\n");
	}
    
	setupDiagram(diagram);
    
	svgtiny_free(diagram);

}

void ofxSVG::draw(){
    
    ofPushMatrix();
    ofTranslate(x,y);
	for(int i = 0; i < paths.size(); i++){
		paths[i]->draw();
	}
    ofPopMatrix();
}


void ofxSVG::setupDiagram(struct svgtiny_diagram * diagram){

	width = diagram->width;
	height = diagram->height;

	for(int i = 0; i < diagram->shape_count; i++){
		if(diagram->shape[i].path){
			setupShape(&diagram->shape[i]);
		}
		else if(diagram->shape[i].text){
			printf("text: not implemented yet\n");
		}
	}
}

void ofxSVG::setupShape(struct svgtiny_shape * shape){
	float * p = shape->path;

	ofPath * path = new ofPath();
	paths.push_back(ofPathRef(path));

	path->setFilled(false);

	if(shape->fill != svgtiny_TRANSPARENT){
		path->setFilled(true);
		path->setFillHexColor(shape->fill);
	}

	if(shape->stroke != svgtiny_TRANSPARENT){
		path->setStrokeWidth(shape->stroke_width);
		path->setStrokeHexColor(shape->stroke);
	}

	for(int i = 0; i < shape->path_length;){
		if(p[i] == svgtiny_PATH_MOVE){
			path->moveTo(p[i + 1], p[i + 2]);
			i += 3;
		}
		else if(p[i] == svgtiny_PATH_CLOSE){
			path->close();

			i += 1;
		}
		else if(p[i] == svgtiny_PATH_LINE){
			path->lineTo(p[i + 1], p[i + 2]);
			i += 3;
		}
		else if(p[i] == svgtiny_PATH_BEZIER){
			path->bezierTo(p[i + 1], p[i + 2],
						   p[i + 3], p[i + 4],
						   p[i + 5], p[i + 6]);
			i += 7;
		}
		else{
			//cout << "error\n" << endl;
			ofLogError() << "SVG parse error";
			i += 1;
		}
	}
}





/*
 These affect the whole document
 */
void ofxSVG::setFilled(bool t){
    
    for(int i=0;i<paths.size();i++){
        paths[i]->setFilled(t);
        if(t){
            setSVGattribute(info.flatlist[i],"fill_opacity","1.0");
        }else{
            setSVGattribute(info.flatlist[i],"fill_opacity","0.0");
        }
    }
    isFilled = t;
   // ofLog()<<"setFilled "<<t<<"  " <<paths.size()<<endl;
    
};


void ofxSVG::setFilled(bool t,int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return;
    }
    paths[path]->setFilled(t);
    
    if(t){
        setSVGattribute(info.flatlist[path],"fill_opacity","1.0");
    }else{
        setSVGattribute(info.flatlist[path],"fill_opacity","0.0");
    }
    
    //ofLog()<<"setFilled path"<<t<<"  " <<path<<endl;

};



void ofxSVG::setFillColor(ofColor col){
    //ofLog()<<"col.r "<<ofToHex(col.r)<<endl;
    for(int i=0;i<paths.size();i++){
        //col.set(255,255,255);
        paths[i]->setFillHexColor(col.getHex());
        //borg...update DOM ...
        setSVGattribute(info.flatlist[i],"fill",hexToWeb(col));
        setSVGattribute(info.flatlist[i],"fill_opacity",ofToString((float) col.a/255.0));
        
        //info.flatlist[i]->path.fill = hexToWeb(col);
        //info.flatlist[i]->path.fill_opacity = ofToString((float) col.a/255.0);
    }
    fill = col;
   // ofLog()<<"setFillColor"<<endl;
};


void ofxSVG::setFillColor(ofColor col,int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return;
    }
    paths[path]->setFillHexColor(col.getHex());
    
    
    setSVGattribute(info.flatlist[path],"fill",ofToString(hexToWeb(col)));
    setSVGattribute(info.flatlist[path],"fill_opacity",ofToString((float) col.a/255.0));
    
    
 //  ofLog()<<"setFillColor path"<<"  " <<path<<" "<<ofToString(hexToWeb(col))<<" info.flatlist.size()  "<<info.flatlist.size()<< " paths.size() "<<paths.size()<<endl;
    
    
    //info.flatlist[path]->path.fill = hexToWeb(col);
   // info.flatlist[path]->path.fill_opacity = ofToString((float) col.a/255.0);
    
};




void ofxSVG::setStrokeWidth(float f){
   // ofLog()<<"setStrokeWidth info.flatlist.size "<< info.flatlist.size()<<" paths.size() "<<paths.size()<<endl;
    
    
    for(int i=0;i<paths.size();i++){
        paths[i]->setStrokeWidth(f);
        //borg...update DOM...make sure you cast correctly as flatlist is using ofPtr...and xcode doesn't tell you float is not string
       
           //info.flatlist[i]->path.stroke_width = ofToString(f);
            setSVGattribute(info.flatlist[i],"stroke_width",ofToString(f));
       

    }
    
    strokeWidth = f;
};
void ofxSVG::setStrokeColor(ofColor col){
    for(int i=0;i<paths.size();i++){
        paths[i]->setStrokeColor(col);
        //borg...update DOM 
        setSVGattribute(info.flatlist[i],"stroke",hexToWeb(col));
        setSVGattribute(info.flatlist[i],"stroke_opacity",ofToString((float) col.a/255.0));
        //info.flatlist[i]->path.stroke = ofToString(hexToWeb(col));
        //info.flatlist[i]->path.stroke_opacity = ofToString((float) col.a/255.0);
    }
    stroke = col;
};



bool ofxSVG::getFilled(){
    return isFilled;
};

ofColor ofxSVG::getFillColor(){
    return fill;
};

float ofxSVG::getStrokeWidth(){
    return strokeWidth;
};

ofColor ofxSVG::getStrokeColor(){
    return stroke;
};



/*
 I wish there was a C++ equivalent of the AS3 this["attr"] syntax. 
 
 */


void ofxSVG::setSVGattribute(ofPtr<svgNode>  node,string attribute, string value){
    switch (node->type){
        case SVG_TAG_TYPE_RECT:
            if(attribute == "x"){
                node->rect.x = value;
            }
            if(attribute == "y"){
                node->rect.y = value;
            }
            if(attribute == "fill"){
                node->rect.fill = value;
            }
            if(attribute == "stroke_width"){
                node->rect.stroke_width = value;
            }
            if(attribute == "stroke"){
                node->rect.stroke = value;
            }
            if(attribute == "width"){
                node->rect.width = value;
            }
            if(attribute == "height"){
                node->rect.height = value;
            }
            if(attribute == "stroke_miterlimit"){
                node->rect.stroke_miterlimit = value;
            }
            if(attribute == "stroke_width"){
                node->rect.stroke_width = value;
            }
            if(attribute == "stroke_opacity"){
                node->rect.stroke_opacity = value;
            }
            if(attribute == "fill_opacity"){
                node->rect.fill_opacity = value;
            }
            
            break;
        case SVG_TAG_TYPE_PATH:
            // ofLog()<<"SVG_TAG_TYPE_PATH"<<endl;
            
            if(attribute == "fill"){
                node->path.fill = value;
            }
            if(attribute == "stroke_width"){
                node->path.stroke_width = value;
            }
            if(attribute == "stroke"){
                node->path.stroke = value;
            }
            if(attribute == "stroke_miterlimit"){
                node->path.stroke_miterlimit = value;
            }
            if(attribute == "stroke_width"){
                node->path.stroke_width = value;
            }
            if(attribute == "stroke_opacity"){
                node->path.stroke_opacity = value;
            }
            if(attribute == "fill_opacity"){
                node->path.fill_opacity = value;
            }
            
            
            break;
        case SVG_TAG_TYPE_GROUP:
            // ofLog()<<"SVG_TAG_TYPE_GROUP"<<endl;
            if(attribute == "transform"){
                node->group.transform = value;
            }
            if(attribute == "fill"){
                node->group.fill = value;
            }
            if(attribute == "stroke_width"){
                node->group.stroke_width = value;
            }
            if(attribute == "stroke"){
                node->group.stroke = value;
            }
            if(attribute == "stroke_miterlimit"){
                node->group.stroke_miterlimit = value;
            }
            if(attribute == "stroke_width"){
                node->group.stroke_width = value;
            }
            if(attribute == "stroke_opacity"){
                node->group.stroke_opacity = value;
            }
            if(attribute == "fill_opacity"){
                node->group.fill_opacity = value;
            }
            break;
    }
            
            
}

void ofxSVG::setStrokeWidth(float f,int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return;
    }
    paths[path]->setStrokeWidth(f);
    setSVGattribute(info.flatlist[path],"stroke_width",ofToString(f));

};

void ofxSVG::setStrokeColor(ofColor col,int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return;
    }
    paths[path]->setStrokeColor(col);
    //borg...update DOM 
    setSVGattribute(info.flatlist[path],"stroke",hexToWeb(col));
    setSVGattribute(info.flatlist[path],"stroke_opacity",ofToString((float) col.a/255.0));
    
    
};

bool ofxSVG::getFilled(int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return 0;
    }
    return paths[path]->isFilled();
};

ofColor ofxSVG::getFillColor(int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return 0;
    }
    return paths[path]->getFillColor();
};

float ofxSVG::getStrokeWidth(int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return 0;
    }
    return paths[path]->getStrokeWidth();
};

ofColor ofxSVG::getStrokeColor(int path){
    if(path>info.flatlist.size()-1){
        ofLog()<<"WARNING: Tried to modify ofxSVG path out of index - "<< path<<" of info.flatlist.size() "<<info.flatlist.size()<<endl;
        return 0;
    }
    return paths[path]->getStrokeColor();
};
/*
 
 Trouble with insert is that it probably removes the original from the previous vector
 http://stackoverflow.com/questions/4122789/copy-object-keep-polymorphism/4122835#4122835
 
 Using pointers in vectors no need to make deep copy
 
 How do I copy translations from merged svg? By adding transformations to a group
 
 The svgs are put into groups.
 
 */
void ofxSVG::merge(ofxSVG & svg){
         //copy children
        ofPtr<svgNode> childnode(new svgNode());
        svgGroupDef gDef;
        gDef.transform = "translate("+ofToString(svg.getX())+","+ofToString(svg.getY())+")";
 
        childnode->group = gDef;  
        childnode->type =SVG_TAG_TYPE_GROUP;
        

        
        childnode->children = svg.info.rootnode->children;
        info.rootnode->children.push_back(childnode);
        
        /*
         reparsing of the whole svg. If correct then all paths etc will be updated.
         */
        
        string str = toString();
        parseXML(str);

   

};

/*
 These values are not used to update paths.
 You need to manually adjust draw pos of whole svg if drawing paths.
 They are however stored in saved svg.
 */
void ofxSVG::setPos(int _x, int _y){
    x = _x;
    y = _y;
};

int ofxSVG::getX(){
    return x;
};
int ofxSVG::getY(){
    return y;
};


// //string because it includes unit, eg, 400pt
void ofxSVG::setSize(int w, int h, string unit){
    width = w;
    height = h;
    info.width = ofToString(w)+unit;
    info.height = ofToString(h)+unit;
}


int ofxSVG::getWidth(){
    return (int) width;
};


int ofxSVG::getHeight(){
    return (int) height;
};



void ofxSVG::setViewbox(int x, int y,int w, int h){
    info.viewbox = ofToString(x)+" " +ofToString(y) +" " +ofToString(w)+" " +ofToString(h);
};

void ofxSVG::save(string file){
    /*
     BORG
     https://jwatt.org/svg/authoring/#doctype-declaration
     
     Wrong DOCTTYPE upsets illustrator. Adviced to leave out.
     
     */


    Document *document = new Document();    
    parseHeader(document);
        
	//DOMParser parser;
    //parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, false);
   
    //document = parser.parseString(info.rawXML);
    //svg = document->documentElement();
    
    
    //Poco::UTF8Encoding utf8encoding;
    //Poco::XML::XMLWriter writer(xmlstream, 0, "UTF-8", &utf8encoding);
    
    DOMWriter writer;
    

    /*
     BORG
     remove unwanted namespace ns1
     this doesn't actually work for some reason.
          
     writer.writeNode(xmlstream, svg);
     string rawXMLstring = xmlstream.str();

    Poco::XML::Document * cleanDocument;
    Poco::XML::DOMParser parser;
    parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACE_PREFIXES, false);
    parser.setFeature(Poco::XML::XMLReader::FEATURE_NAMESPACES, false);
       
    cleanDocument = parser.parseString(rawXMLstring);

      */
    string fullXmlFile;
   /* if(OF_TARGET_IPHONE){
    //how to use. Fix to make compatible with ios directly
        fullXmlFile = ofToDataPath("../Documents/"+file);
    }else{*/
        fullXmlFile = ofToDataPath(file);
  //  }
    writer.setNewLine("\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.writeNode(fullXmlFile, document);
    ofLog()<<"Saved "<<fullXmlFile<<endl;
}


string ofxSVG::toString(){    
    Document *document = new Document();
    
    parseHeader(document);
    DOMWriter writer;
    stringstream xmlstream;
    writer.setNewLine("\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.writeNode(xmlstream, document);
    
    return xmlstream.str();
    
}

void ofxSVG::parseHeader(Document * document){
    
    Element *svg = document->createElement("svg");
    if(!info.x.empty()){
        svg->setAttribute("x",info.x);
    }
    if(!info.y.empty()){
        svg->setAttribute("y",info.y);
    }
    if(!info.width.empty()){
        svg->setAttribute("width",info.width);
    }
    if(!info.height.empty()){
        svg->setAttribute("height",info.height);
    }
    if(!info.enable_background.empty()){
        svg->setAttribute("enable-background",info.enable_background);
    }
    if(!info.viewbox.empty()){
        svg->setAttribute("viewBox",info.viewbox);
    }
    if(!info.version.empty()){
        svg->setAttribute("version",info.version);
    }
    if(!info.id.empty()){
        svg->setAttribute("id",info.id);
    }
    if(!info.preserveAspectRatio.empty()){
        svg->setAttribute("preserveAspectRatio",info.preserveAspectRatio);
    }
    if(!info.xmlns.empty()){
        svg->setAttribute("xmlns",info.xmlns);
    }else{
        svg->setAttribute("xmlns","http://www.w3.org/2000/svg");
        
    }
    
    // ofLog()<<"info.preserveAspectRatio "<<info.preserveAspectRatio<<endl;
    //svg->setAttribute("xml:space",info.xml_space);
    //svg->setAttribute("xmlns",info.xmlns);
    //svg->setAttribute("xmlns:ns1",info.xmlns_ns1);
    //svg->setAttribute("xmlns:xlink",info.xmlns_xlink);
    
    
    document->appendChild(svg);
    
    if(info.rootnode){
        xmlCreateSVG(document,svg,info.rootnode);
    }else{
        ofLog()<<"check info.rootnode health"<<endl;
    }
       
    

    
};


Element * ofxSVG::parseNode(Document *doc,ofPtr<svgNode> node){
    
    if(!node){
        ofLog()<<"Why is node null?"<<endl;
        return 0;
    }
    
    Element * newNode;
    switch (node->type){
        case SVG_TAG_TYPE_RECT:
            newNode = doc->createElement("rect");
            // ofLog()<<"node->path.fill "<<node->path.fill<<endl;
            if(!node->rect.x.empty()){
                newNode->setAttribute("x",node->rect.x);
            }
            if(!node->rect.y.empty()){
                newNode->setAttribute("y",node->rect.y);
            }
            if(!node->rect.width.empty()){
                newNode->setAttribute("width",node->rect.width);
            }
            if(!node->rect.height.empty()){
                newNode->setAttribute("height",node->rect.height);
            }
            if(!node->rect.fill.empty()){
                newNode->setAttribute("fill",node->rect.fill);
            }
            if(!node->rect.stroke.empty()){
                newNode->setAttribute("stroke",node->rect.stroke);
            }
            if(!node->rect.stroke_miterlimit.empty()){
                newNode->setAttribute("stroke-miterlimit",node->rect.stroke_miterlimit);
            }
            if(!node->rect.stroke_width.empty()){
                newNode->setAttribute("stroke-width",node->rect.stroke_width);
            }
            if(!node->rect.fill_opacity.empty()){
                newNode->setAttribute("fill-opacity",node->rect.fill_opacity);
            }
            if(!node->rect.stroke_opacity.empty()){
                newNode->setAttribute("stroke-opacity",node->rect.stroke_opacity);
            }
            break;
        case SVG_TAG_TYPE_PATH:
            newNode = doc->createElement("path");
            newNode->setAttribute("d",node->path.d);
            //ofLog()<<"parse node->path.fill "<<node->path.fill<<endl;
            if(!node->path.fill.empty()){
                newNode->setAttribute("fill",node->path.fill);
            }
            if(!node->path.stroke.empty()){
                newNode->setAttribute("stroke",node->path.stroke);
            }
            if(!node->path.stroke_miterlimit.empty()){
                newNode->setAttribute("stroke-miterlimit",node->path.stroke_miterlimit);
            }
            if(!node->path.stroke_width.empty()){
                newNode->setAttribute("stroke-width",node->path.stroke_width);
            }
            if(!node->path.fill_opacity.empty()){
                newNode->setAttribute("fill-opacity",node->path.fill_opacity);
            }
            if(!node->path.stroke_opacity.empty()){
                newNode->setAttribute("stroke-opacity",node->path.stroke_opacity);
            }
            break;
        case SVG_TAG_TYPE_GROUP:
           // ofLog()<<"SVG_TAG_TYPE_GROUP"<<endl;
            newNode = doc->createElement("g");
            if(!node->group.transform.empty()){
                newNode->setAttribute("transform",node->group.transform);
            }
            if(!node->group.fill.empty()){
                newNode->setAttribute("fill",node->group.fill);
            }
            if(!node->group.stroke.empty()){
                newNode->setAttribute("stroke",node->group.stroke);
            }
            if(!node->group.stroke_miterlimit.empty()){
                newNode->setAttribute("stroke-miterlimit",node->group.stroke_miterlimit);
            }
            if(!node->group.stroke_width.empty()){
                newNode->setAttribute("stroke-width",node->group.stroke_width);
            }
            if(!node->group.fill_opacity.empty()){
                newNode->setAttribute("fill-opacity",node->group.fill_opacity);
            }
            if(!node->group.stroke_opacity.empty()){
                newNode->setAttribute("stroke-opacity",node->group.stroke_opacity);  
            }
            break;
        case SVG_TAG_TYPE_SVG:
           // ofLog()<<"SVG_TAG_TYPE_SVG"<<endl;
            newNode = doc->createElement("svg");
            newNode->setAttribute("x",node->svg.x);
            newNode->setAttribute("y",node->svg.y);
            newNode->setAttribute("width",node->svg.width);
            newNode->setAttribute("height",node->svg.height);
            if(!node->svg.enable_background.empty()){
                newNode->setAttribute("enable-background",node->svg.enable_background);
            }
            
           // newNode->setAttribute("viewBox",node->svg.viewbox);
            break;
        case SVG_TAG_TYPE_DOCUMENT:
            //ofLog()<<"SVG_TAG_TYPE_DOCUMENT"<<endl;
            //we are ignoring nested svg attributes ...its just that the doc root is xml root...and stuff....whatever
            return 0;
            /*
            newNode = doc->createElement("svg");
            newNode->setAttribute("x",node->svg.x);
            newNode->setAttribute("y",node->svg.y);
            newNode->setAttribute("width",node->svg.width);
            newNode->setAttribute("height",node->svg.height);
            if(!node->svg.enable_background.empty()){
                newNode->setAttribute("enable-background",node->svg.enable_background);
            }
             */
            break;
            
    }
    return newNode;
};

void ofxSVG::xmlCreateSVG(Document *doc,Element *extNode, ofPtr<svgNode> intNode){    
    Element * newNode = parseNode(doc,intNode);
    if(newNode!=0){
        extNode->appendChild(newNode);
        //recursive call
        for(int i=0;i<intNode->children.size();i++){
            xmlCreateSVG(doc,newNode,intNode->children[i]);
        }
    }else{
        //recursive call
        for(int i=0;i<intNode->children.size();i++){
            xmlCreateSVG(doc,extNode,intNode->children[i]);
        }
    }
        
   

};





