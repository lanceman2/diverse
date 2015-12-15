#include <dtk.h>
#include <dtk/dtkDSO_loader.h>
#include <dgl.h>

#define NODES (4)

/*!
 * @class  dosgToggleObjectsKeyboardControl
 *
 * @brief DSO which passes messages to the dosgToggleObjects DSO based on
 * key presses
 *
 * Each object uses a modifier key to specify what under node the toggle
 * object should be placed
 * -  'w' or 'W'
 *   - under the "world" node
 * -  'e' or 'E'
 *   - under the "ether" node
 * -  's' or 'S'
 *   - under the "scene" node
 * -  'n' or 'N'
 *   - under the "nav" node
 *
 * The objects are:
 * -  'c' or 'C'
 *   - cube- four presses to cycle through frame and panel
 * -  'f' or 'F'
 *   - frame
 * -  'g' or 'G'
 *   - gnomon
 * -  'a' or 'A'
 *   - axis
 * -  't' or 'T'
 *   - text label
 *
 * 'Backspace' turns off all toggle objects
 *
 * DGLMessage data is send using a DGLPostOffice using the dtk shared memory
 * file "dgl/dosgToggleObjects"
 *
 * @author John Kelso, kelso@nist.gov
 * @author Andrew A. Ray, anray2@vt.edu
 */
class dosgToggleObjectsKeyboardControl : public dtkAugment
{
    public:
        dosgToggleObjectsKeyboardControl(dtkManager *);

        virtual int postFrame(void) ;
        virtual void makeMsg(int idx, KeyCharacter A) ;
        virtual void sendMsg(int idx, string obj, int v) ;
        virtual void reset(void) ;

    private:

        DGLKeyboard   *kb ;
        DGLMessage    *msg ;
        DGLPostOffice *msgSender ;

        enum NODENAME { SCENE=0, ETHER=1, NAV=2, WORLD=3 }
        ;
        class objVis
        {
            public:
                void clear(void)
                    { cube = frame = axis = gnomon = text = 0 ; }
                ;
                int cube ;
                int frame ;
                int axis ;
                int gnomon ;
                int text ;
                char *name ;
        } ;

        objVis node[NODES] ;

};

dosgToggleObjectsKeyboardControl::dosgToggleObjectsKeyboardControl(dtkManager *m) :
dtkAugment("dosgToggleObjectsKeyboardControl")
{
    setDescription("reads the keyboard and sets dtk shared memory segments to "
        "control the dosg toggle objects") ;

    kb = new DGLKeyboard ;

//Class for sending the message
    msgSender = new DGLPostOffice("dgl/dosgToggleObjects") ;
    msg = new DGLMessage ;

    node[SCENE].name = strdup("scene") ;
    node[ETHER].name = strdup("ether") ;
    node[NAV].name = strdup("nav") ;
    node[WORLD].name = strdup("world") ;

    validate() ;
}


void dosgToggleObjectsKeyboardControl::reset(void)
{
    for (int i = 0; i<NODES; i++)
    {
        node[i].clear() ;
        sendMsg(i, "cubeFrame", 0) ;
        sendMsg(i, "cubePanel", 0) ;
        sendMsg(i, "frame", 0) ;
        sendMsg(i, "gnomon", 0) ;
        sendMsg(i, "axis", 0) ;
        sendMsg(i, "text", 0) ;
    }
}


void dosgToggleObjectsKeyboardControl::sendMsg(int idx, string obj, int v)
{
//printf("sendMsg(\"%s\", \"%s\", %d)\n",node[idx].name, obj.c_str(), v) ;
    msg->clear() ;
    msg->setSubject(node[idx].name);
    msg->addString(obj) ;
    msg->addInt(v);
    msgSender->sendMessage(msg);
}


void dosgToggleObjectsKeyboardControl::makeMsg(int idx, KeyCharacter A)
{
    if (A == KeyChar_c || A == KeyChar_C)
    {
        node[idx].cube = (node[idx].cube+1)%4 ;
        if (node[idx].cube == 0)
        {
            sendMsg(idx, "cubeFrame", 0) ;
            sendMsg(idx, "cubePanel", 0) ;
        }
        else if (node[idx].cube == 1)
        {
            sendMsg(idx, "cubeFrame", 1) ;
//sendMsg(idx, "cubePanel", 0) ;
        }
        else if (node[idx].cube == 2)
        {
            sendMsg(idx, "cubeFrame", 0) ;
            sendMsg(idx, "cubePanel", 1) ;
        }
        else if (node[idx].cube == 3)
        {
            sendMsg(idx, "cubeFrame", 1) ;
//sendMsg(idx, "cubePanel", 1) ;
        }
    }
    else if (A == KeyChar_f || A == KeyChar_F)
    {
        node[idx].frame = (node[idx].frame+1)%2 ;
        sendMsg(idx, "frame", node[idx].frame) ;
    }
    else if (A == KeyChar_g || A == KeyChar_G)
    {
        node[idx].gnomon = (node[idx].gnomon+1)%2 ;
        sendMsg(idx, "gnomon", node[idx].gnomon) ;
    }
    else if (A == KeyChar_a || A == KeyChar_A)
    {
        node[idx].axis = (node[idx].axis+1)%2 ;
        sendMsg(idx, "axis", node[idx].axis) ;
    }
    else if (A == KeyChar_t || A == KeyChar_T)
    {
        node[idx].text = (node[idx].text+1)%2 ;
        sendMsg(idx, "text", node[idx].text) ;
    }
}


int dosgToggleObjectsKeyboardControl::postFrame(void)
{

    static bool first = true ;
    if (first)
    {
//reset() ;
        first = false ;
    }

    bool s = (DGLKeyboard::getState(KeyChar_s)==DGLKeyboard::PRESSED ||
        DGLKeyboard::getState(KeyChar_S)==DGLKeyboard::PRESSED) ;

    bool e = (DGLKeyboard::getState(KeyChar_e)==DGLKeyboard::PRESSED ||
        DGLKeyboard::getState(KeyChar_E)==DGLKeyboard::PRESSED) ;

    bool w = (DGLKeyboard::getState(KeyChar_w)==DGLKeyboard::PRESSED ||
        DGLKeyboard::getState(KeyChar_W)==DGLKeyboard::PRESSED) ;

    bool n = (DGLKeyboard::getState(KeyChar_n)==DGLKeyboard::PRESSED ||
        DGLKeyboard::getState(KeyChar_N)==DGLKeyboard::PRESSED) ;

    DGLKeyboard::ACTION a ;
    KeyCharacter A = kb->readKeyCharacter(&a) ;

    if (A && a==DGLKeyboard::RELEASED)
    {
        if (s)
            makeMsg(SCENE, A) ;
        else if (e)
            makeMsg(ETHER, A) ;
        else if (n)
            makeMsg(NAV, A) ;
        else if (w)
            makeMsg(WORLD, A) ;
        else if (A == KeyChar_BackSpace)
            reset() ;
    }

    return CONTINUE;
}


/************ DTK C++ dtkAugment loader/unloader functions ***************
 *
 * All DTK dtkAugment DSO files are required to declare these two
 * functions.  These function are called by the loading program to get
 * your C++ objects loaded.
 *
 *************************************************************************/

static dtkAugment *dtkDSO_loader(dtkManager *manager, void *p)
{
    return new dosgToggleObjectsKeyboardControl(manager);
}


static int dtkDSO_unloader(dtkAugment *augment)
{
    delete augment;
    return DTKDSO_UNLOAD_CONTINUE;
}
