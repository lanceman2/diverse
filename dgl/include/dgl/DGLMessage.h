#ifndef dglmessage_h
#define dglmessage_h

#include <string>
#include <sstream>
using namespace std;

/*!
 * @class DGLMessage DGLMessage.h dgl.h
 *
 * @brief A class for storing messages
 * 
 * A message contains a sender, recipient, subject, and an arbitrary number
 * of values of all the usual types- int, float, float, string and char.
 * Useful in general for Diverse applications
 *
 * Warning, do not use tabs in your messages, spaces are OK though.
 *
 * @author Andrew A. Ray, anray2@vt.edu
 * @author Mark Soltys
 * @author John Kelso, kelso@nist.gov
 */
class DGLAPI DGLMessage
{
 public:

  /*!
   * Construct an empty message
   */
  DGLMessage();

  // usual destructor
  ~DGLMessage();

  /*!
   * Construct a message from a string
   */
  DGLMessage(const string& Input);
	
  /*!
   * Construct a message from a another DGLMessage
   */
  DGLMessage(DGLMessage& Input);

  /*!
   * equals operator
   */
  DGLMessage operator=(DGLMessage& Other);

  //Message header functions
  
  /*!
   * @brief Return the "from" field
   *
   * @return the "from" field
   */
  string getFrom();

  /*!
   * @brief Set the "from" field
   *
   * @param From the "from" field
   */
  void   setFrom(const string& From);
  
  /*!
   * @brief Return the "to" field
   *
   * @return the "to" field
   */
  string getTo();
	
  /*!
   * @brief Set the "to" field
   *
   * @param To the "to" field
   */
  void   setTo(const string& To);

  /*!
   * @brief Return the "subject" field
   *
   * @return the "subject" field
   */
  string getSubject();
	
  /*!
   * @brief Set the "subject" field
   *
   * @param Subject the "subject" field
   */
  void   setSubject(const string& Subject);

  //Parameter functions

  /*!
   * @brief Add an "int" to the message
   *
   * @param Value the "int" to add to the message
   */
  void addInt(const int& Value);

  /*!
   * @brief Return next item from the message, interpreting it as an "int"
   *
   * @return next item from the message, interpreting it as an "int"
   */
  int  getInt();

  /*!
   * @brief Add a "float" to the message
   *
   * @param Value the "float" to add to the message
   */
  void addFloat(const float& Value);

  /*!
   * @brief Return next item from the message, interpreting it as a "float"
   *
   * @return next item from the message, interpreting it as a "float"
   */
  float getFloat();

  /*!
   * @brief Add a "double" to the message
   *
   * @param Value the "double" to add to the message
   */
  void addDouble(const double& Value);

  /*!
   * @brief Return next item from the message, interpreting it as a "double"
   *
   * @return next item from the message, interpreting it as a "double"
   */
  double getDouble();

  /*!
   * @brief Add a "string" to the message
   *
   * @param Value the "string" to add to the message
   */
  void addString(const string& Value);

  /*!
   * @brief Return next item from the message, interpreting it as a "string"
   *
   * @return next item from the message, interpreting it as a "string"
   */
  string getString();

  /*!
   * @brief Add a "char" to the message
   *
   * @param character the "char" to add to the message
   */
  void addChar(const char& character);

  /*!
   * @brief Return next item from the message, interpreting it as a "char"
   *
   * @return next item from the message, interpreting it as a "char"
   */
  char getChar();

  /*!
   * @brief Return the entire message as a single string
   *
   * @return the entire message as a single string
   */
  string getMessage();

  /*!
   * @brief Clear the message
   */
  void clear();

 private:
  string m_from;
  string m_to;
  string m_subject;
  stringstream* m_parameters;
};
#endif
