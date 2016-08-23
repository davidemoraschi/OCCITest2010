#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/DOM/ProcessingInstruction.h"
#include "ocl.h"
#include "oramonitor.h"
#include <iostream>

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using Poco::XML::Document;
using Poco::XML::Element;
using Poco::XML::Text;
using Poco::XML::AutoPtr;
using Poco::XML::DOMWriter;
using Poco::XML::XMLWriter;
using Poco::XML::ProcessingInstruction;

using namespace ocl;

class TimeRequestHandler: public HTTPRequestHandler
{
public:
    TimeRequestHandler(const std::string& format): _format(format)
    {
    }

    void handleRequest(HTTPServerRequest& request,
                       HTTPServerResponse& response)
    {
        Application& app = Application::instance();
        app.logger().information("Request from "
            + request.clientAddress().toString());

        Timestamp now;
        std::string dt(DateTimeFormatter::format(now, _format));
		
		OraConnection connection;
		OraQuery query(connection);
		long recordCount = 0L;
		char* connectString = "cdm/iPaddellaSamsung@fraterno:1521/mti.valme.net";  // replace ora with your TNS alias
		connection.setConnectString(connectString);  // set login information

		OraMonitor::start();
		connection.open();  // establish connection

        response.setChunkedTransferEncoding(true);
        response.setContentType("text/xml");

        std::ostream& ostr = response.send();

		AutoPtr<Document> pDoc = new Document;
		
		AutoPtr<ProcessingInstruction> p1 = pDoc->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
		AutoPtr<ProcessingInstruction> p2 = pDoc->createProcessingInstruction("xml-stylesheet", "type=\"text/xsl\" href=\"http://fraterno/ADO_Persisted.xsl\"");
//		AutoPtr<ProcessingInstruction> p1 = pDoc->createProcessingInstruction("xml-stylesheet", "type=\"text/xsl\" href=\"file://C:\\TEMP\\ADO_Peristed.xsl\"");
		//<?xml version="1.0" encoding="UTF-8" ?>
		pDoc->appendChild(p1);
		pDoc->appendChild(p2);

		AutoPtr<Element> pRoot = pDoc->createElement("xml");
		pRoot->setAttribute("xmlns:s", "uuid:BDC6E3F0-6DA3-11d1-A2A3-00AA00C14882");
		pRoot->setAttribute("xmlns:dt", "uuid:C2F41010-65B3-11d1-A29F-00AA00C14882");
		pRoot->setAttribute("xmlns:rs", "urn:schemas-microsoft-com:rowset");
		pRoot->setAttribute("xmlns:z", "#RowsetSchema");
		pDoc->appendChild(pRoot);

		AutoPtr<Element> pSchema = pDoc->createElement("s:Schema");
		pSchema->setAttribute("id", "RowsetSchema");

		AutoPtr<Element> pElementType = pDoc->createElement("s:ElementType");
		pElementType->setAttribute("name", "row");
		pElementType->setAttribute("content", "eltOnly");

		AutoPtr<Element> pAttributeType = pDoc->createElement("s:AttributeType");
		pAttributeType->setAttribute("name", "Origen");
		pAttributeType->setAttribute("rs:number", "1");
		pAttributeType->setAttribute("rs:nullable", "true");
		pAttributeType->setAttribute("rs:maydefer", "true");
		pAttributeType->setAttribute("rs:writeunknown", "true");

		AutoPtr<Element> pdatatype = pDoc->createElement("s:datatype");
		pdatatype->setAttribute("dt:type", "string");
		pdatatype->setAttribute("dt:maxLength", "255");

		pAttributeType->appendChild(pdatatype);
		pElementType->appendChild(pAttributeType);
		pSchema->appendChild(pElementType);
		pRoot->appendChild(pSchema);

		AutoPtr<Element> pdata = pDoc->createElement("rs:data");


		//query.setCommandText("SELECT * FROM HTML_RSS_NEWS");
		query.setSQL("SELECT SUBSTR(TITLE, 0, 80) TITLE FROM HTML_RSS_NEWS");
		query.setCached(false);  // noncached mode
		query.setFetchRows(10);

		query.open();
		while (!query.isEOF()) {
					AutoPtr<Element> prow = pDoc->createElement("z:row");
					prow->setAttribute("Origen", query.field("TITLE").getChars());
					pdata->appendChild(prow);
					recordCount++;
		//cout << "TITLE: " << query.field("TITLE").getString() << endl;
		query.next();
		}
		query.close();
		connection.close();  // close connection
		OraMonitor::stop();

		pRoot->appendChild(pdata);

		//AutoPtr<Element> pChild2 = pDoc->createElement("row");
		//AutoPtr<Text> pText2 = pDoc->createTextNode("text2");
		//pChild2->appendChild(pText2);
		//pRoot->appendChild(pChild2);
		
		DOMWriter writer;
		writer.setNewLine("\n");
		writer.setOptions(XMLWriter::PRETTY_PRINT);
		writer.writeNode(ostr, pDoc);

		//connection.close();

    }

private:
    std::string _format;
};

class TimeRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    TimeRequestHandlerFactory(const std::string& format):
        _format(format)
    {
    }

    HTTPRequestHandler* createRequestHandler(
        const HTTPServerRequest& request)
    {
        if (request.getURI() == "/")
            return new TimeRequestHandler(_format);
        else
            return 0;
    }

private:
    std::string _format;
};

class HTTPTimeServer: public Poco::Util::ServerApplication
{
public:
    HTTPTimeServer(): _helpRequested(false)
    {
    }

    ~HTTPTimeServer()
    {
    }

protected:
    void initialize(Application& self)
    {
        loadConfiguration();
        ServerApplication::initialize(self);
    }

    void uninitialize()
    {
        ServerApplication::uninitialize();
    }

    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);

        options.addOption(
        Option("help", "h", "display argument help information")
            .required(false)
            .repeatable(false)
            .callback(OptionCallback<HTTPTimeServer>(
                this, &HTTPTimeServer::handleHelp)));
    }

    void handleHelp(const std::string& name, 
                    const std::string& value)
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader(
            "A web server that serves the current date and time.");
        helpFormatter.format(std::cout);
        stopOptionsProcessing();
        _helpRequested = true;
    }

    int main(const std::vector<std::string>& args)
    {
        if (!_helpRequested)
        {
            unsigned short port = (unsigned short)
                config().getInt("HTTPTimeServer.port", 9980);
            std::string format(
                config().getString("HTTPTimeServer.format", 
                                   DateTimeFormat::SORTABLE_FORMAT));

            ServerSocket svs(port);
            HTTPServer srv(new TimeRequestHandlerFactory(format), 
                svs, new HTTPServerParams);
            srv.start();
            waitForTerminationRequest();
            srv.stop();
        }
        return Application::EXIT_OK;
    }

private:
    bool _helpRequested;
};

int main(int argc, char** argv)
{
    HTTPTimeServer app;
    return app.run(argc, argv);
}
