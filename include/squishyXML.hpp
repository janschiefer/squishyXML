//============================================================================
// Name        : squishyXML.hpp
// Author      : Jan Schiefer
// Version     : 0.1
// Copyright   : GNU Lesser General Public License (LGPL)
// Description : A dead easy libxml2 wrapper in C++/RAII-style.
//============================================================================
#ifndef SQUISHYXML_HPP_
#define SQUISHYXML_HPP_

#include <string>
#include <vector>
#include <unordered_map>
#include <libxml/xmlreader.h>
#include <libxml/parser.h>

class squishyXMLParser {

public:
	squishyXMLParser();
	~squishyXMLParser();

};

class squishyXMLContext {

public:
	squishyXMLContext();
	~squishyXMLContext();

	xmlParserCtxtPtr ptr;

};

class squishyXMLDocument {

public:
	squishyXMLDocument( squishyXMLContext &context, std::string_view filename, std::string_view encoding, int options );
	squishyXMLDocument ( std::string_view version );
	~squishyXMLDocument();

	void setRootElement( xmlNode *rootNode );
	xmlNode *getRootElement();

	void printDocToString( std::string &result, std::string_view encoding ,bool addFormattingSpaces, bool withXMLDecl );

	xmlDocPtr ptr;

};

class squishyXMLNodeUtil {

public:
	squishyXMLNodeUtil();
	~squishyXMLNodeUtil();

	static xmlNode *createNewNode( xmlNs *n_space, std::string_view name );
	static void unlinkNode ( xmlNode *node, bool freeData = true );

	static xmlNode *findSingleNodeByName( std::string_view nodeName, xmlNode *startNode );
	static xmlNode *findSingleChildNodeByName( std::string_view nodeName, xmlNode *startNode );

	static void findNodesByName( std::string_view nodeName, xmlNode *startNode, std::vector<xmlNode *>  &results );
	static void findChildNodesByName( std::string_view nodeName, xmlNode *startNode, std::vector<xmlNode *>  &results );

	static bool getNodeContent( const xmlNode *node, std::string &result );
	static bool getNodeProperty( const xmlNode *node, std::string_view name, std::string &result );
	static void getNodeProperties( const xmlNode *node, std::unordered_map <std::string, std::string> &result );

	static void setNodeContent( xmlNode *node, std::string_view content );
	static void changeNodeName ( xmlNode *node, std::string_view name );
	static void setNodeProperty( xmlNode *node, std::string_view key, std::string_view value );
	static void setNodeProperties( xmlNode *node, const std::unordered_map <std::string, std::string> &properties );
	static bool addChildNode( xmlNode *parent, xmlNode *child );

};



#endif /* SQUISHYXML_HPP_ */
