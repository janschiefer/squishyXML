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

class squishyXMLNode {

public:
	squishyXMLNode();
	squishyXMLNode( xmlNs *n_space, std::string_view name );

	~squishyXMLNode();

	void setNodePointer( xmlNode *node );
	void unlinkNode ( bool freeNode = false );

	bool findSingleNodeByName( std::string_view nodeName, squishyXMLNode &result, bool searchChildren );

	bool findNodesByName( std::string_view nodeName, std::vector<squishyXMLNode> &results, bool searchChildren );

	bool getNodeContent( std::string &result );
	bool getNodeProperty( std::string_view name, std::string &result );
	bool getNodeProperties( std::unordered_map <std::string, std::string> &result );

	bool setNodeContent( std::string_view content );
	bool setNodeProperty( std::string_view key, std::string_view value );
	bool setNodeProperties( const std::unordered_map <std::string, std::string> &properties );

	bool changeNodeName ( std::string_view name );

	bool addChildNode( squishyXMLNode &child );

	xmlNode *ptr = NULL;

};

class squishyXMLDocument {

public:
	squishyXMLDocument( squishyXMLContext &context, std::string_view filename, std::string_view encoding, int options );
	squishyXMLDocument ( std::string_view version );
	~squishyXMLDocument();

	bool setRootElement( squishyXMLNode &node );
	bool getRootElement( squishyXMLNode &node );

	bool printDocToString( std::string &result, std::string_view encoding ,bool addFormattingSpaces, bool withXMLDecl );
	bool printDocToFile( std::string_view filename, std::string_view encoding ,bool addFormattingSpaces, bool withXMLDecl );

	xmlDocPtr ptr;

};

#endif /* SQUISHYXML_HPP_ */
