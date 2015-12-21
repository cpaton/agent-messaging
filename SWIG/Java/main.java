public class main {
	//load the agent plaform shared library
	static {
		System.loadLibrary("agentPlatform");
	}
	
	public static void main(String[] args) {	
		new main(args);
	}
	
	//decide which agent action to performm
	public main(String[] args) {
		//chose which agent test to run based on the arguments passed in
		if (args[0].compareToIgnoreCase("server")  == 0) {
			agentPlatform.bootstrapPlatform();
		}
		else if (args[0].compareToIgnoreCase("agent")  == 0) {
			runTestAgent(args[1]);
		}
		else if (args[0].compareToIgnoreCase("api")  == 0) {
			testAPI();
		}
		else {
			System.out.println("Unknown action to perform");
		}
	}
	
	private SWIGTYPE_p_ACLMessage buildMessage() {
		//create a new ACL message
		SWIGTYPE_p_ACLMessage ACLMessage = agentPlatform.ACLMessageNew("query-if");		
		
		//set some of the fields
		ACLMessage = agentPlatform.SACLMessageSetContent(ACLMessage, "(ping)");
		ACLMessage = agentPlatform.SACLMessageSetEncoding(ACLMessage, "std.string");
		ACLMessage = agentPlatform.SACLMessageSetLanguage(ACLMessage, "string");
		ACLMessage = agentPlatform.SACLMessageSetOntology(ACLMessage, "ap-tests");
		
		//add a receiver
		SWIGTYPE_p_AID id = agentPlatform.AIDNew();
		id = agentPlatform.SAIDSetName(id, "server");
		ACLMessage = agentPlatform.SACLMessageAddReceiver(ACLMessage, id);
		
		return ACLMessage;		
	}
	
	private void runTestAgent(String agentName) {
		//build the error structure
		SWIGTYPE_p_APError error = agentPlatform.APNewError();
		
		//connect to the platform
		SWIGTYPE_p_AgentConfiguration myAgent;		
		myAgent = agentPlatform.AP_newAgent(agentName, error);
		if (agentPlatform.SAPErrorIsSet(error) != 1) 
			System.out.println("Agent bootstrapped successfully");
		else {
			System.out.println("Recevied error while bootstrapping - " + agentPlatform.gstrToString(agentPlatform.SAPErrorGetError(error)));
			System.exit(1);
		}
		
		//now build a message to send
		SWIGTYPE_p_ACLMessage msg = buildMessage();
		
		//print the message to the screen
		SWIGTYPE_p_GString gstring = agentPlatform.ACLMessageToString(msg);
		String str = agentPlatform.gstrToString(gstring);
		System.out.println(str);
		
		//now send the message
		agentPlatform.AP_send(myAgent, msg, error);
		if (agentPlatform.SAPErrorIsSet(error) != 1) 
			System.out.println("Message Sent");
		else {
			System.out.println("Recevied error while sending message - " + agentPlatform.gstrToString(agentPlatform.SAPErrorGetError(error)));
			System.exit(1);
		}
		
		//now hang around for 10 seconds for a reply
		try {
			System.out.println("Sleeping");
			//Thread.wait(1000);
			wait();
		}
		catch (Exception e) {
			System.out.println("Error " + e.toString());
		}
		
		agentPlatform.AP_agentSleep(myAgent);
		agentPlatform.AP_finish(myAgent, error);
	}
	
	private void testAPI() {
		testACLMessage();
	}
	
	private void testACLMessage() {
		//create a new ACL message
		SWIGTYPE_p_ACLMessage ACLMessage = agentPlatform.ACLMessageNew("request");		
		
		//set some of the fields
		ACLMessage = agentPlatform.SACLMessageSetContent(ACLMessage, "(ping)");
		ACLMessage = agentPlatform.SACLMessageSetEncoding(ACLMessage, "std.string");
		ACLMessage = agentPlatform.SACLMessageSetLanguage(ACLMessage, "string");
		ACLMessage = agentPlatform.SACLMessageSetOntology(ACLMessage, "ap-tests");
		
		//add a receiver
		SWIGTYPE_p_AID id = agentPlatform.AIDNew();
		id = agentPlatform.SAIDSetName(id, "server");
		ACLMessage = agentPlatform.SACLMessageAddReceiver(ACLMessage, id);
		
		//print out the ACL Message
		SWIGTYPE_p_GString gstring = agentPlatform.ACLMessageToString(ACLMessage);
		String str = agentPlatform.gstrToString(gstring);
		System.out.println(str);
	}
}