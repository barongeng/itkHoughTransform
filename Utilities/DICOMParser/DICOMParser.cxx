
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <stdlib.h>
#if !defined(__MWERKS__)
#include <math.h>
#endif
#include <time.h>
#include <assert.h>
#if !defined(__MWERKS__)
#include <sys/types.h>
#endif

#include <string>
#include <fstream>
#include <map>
#include <iomanip>


#include "DICOMParser.h"
#include "DICOMCallback.h"


// Define DEBUG_DICOM to get debug messages sent to std::cerr
// #define DEBUG_DICOM

#define DICOMPARSER_IGNORE_MAGIC_NUMBER

#ifdef DEBUG_DICOM
#define DICOM_DBG_MSG(x) {std::cout x}
#else 
#define DICOM_DBG_MSG(x)
#endif

static const char* DICOM_MAGIC = "DICM";
static const int   OPTIONAL_SKIP = 128;


DICOMParser::DICOMParser() : ParserOutputFile()
{
  DataFile = NULL;
  this->ToggleByteSwapImageData = false;
  this->InitTypeMap();
}

bool DICOMParser::OpenFile(char* filename)
{
  DataFile = new DICOMFile();
  bool val = DataFile->Open(filename);

  if (this->ParserOutputFile.is_open())
    {
    this->ParserOutputFile.flush();
    this->ParserOutputFile.close();
    }

  std::string fn(filename);
  std::string append(".parser.txt");
  std::string parseroutput(fn + append);
  // std::string parseroutput(std::string(std::string(filename) + std::string(".parser.txt")));
  this->ParserOutputFile.open(parseroutput.c_str()); //, std::ios::app);
  
  return val;
}

DICOMParser::~DICOMParser() {
  this->ParserOutputFile.flush();
  this->ParserOutputFile.close();
  if (DataFile)
    {
    delete DataFile;
    }
}

bool DICOMParser::ReadHeader() {
  bool dicom = this->IsDICOMFile(this->DataFile);
  if (!dicom)
    {
    return false;
    }

  DICOMMemberCallback<DICOMParser>* cb4 = new DICOMMemberCallback<DICOMParser>;
  cb4->SetCallbackFunction(this, &DICOMParser::TransferSyntaxCallback);
  this->AddDICOMTagCallback(0x0002, 0x0010, DICOMParser::VR_UI, cb4);

  this->ToggleByteSwapImageData = false;

  doublebyte group = 0;
  doublebyte element = 0;

  do 
    {
    this->ReadNextRecord(group, element);
    } while (DataFile->Tell() < DataFile->GetSize());


  return true;
}

//
// read magic number from file
// return true if this is your image type, false if it is not
//
bool DICOMParser::IsDICOMFile(DICOMFile* file) {
  char magic_number[4];    
  file->SkipToStart();
  file->Read((void*)magic_number,4);
  if (CheckMagic(magic_number)) 
    {
    return(true);
    } 
  // try with optional skip
  else 
    {
    file->Skip(OPTIONAL_SKIP-4);
    file->Read((void*)magic_number,4);
    if (CheckMagic(magic_number)) 
      {
      return true;
      }
    else
      {

#ifndef DICOMPARSER_IGNORE_MAGIC_NUMBER
      return false;
#else
      //
      // Try it anyways...
      //

      file->SkipToStart();

      doublebyte group = file->ReadDoubleByte();
      bool dicom;
      if (group == 0x0002 || group == 0x0008)
        {
        std::cerr << "No DICOM magic number found, but file appears to be DICOM." << std::endl;
        std::cerr << "Proceeding without caution."  << std::endl;
        dicom = true;
        }
      else
        {
        dicom = false;
        }
      file->SkipToStart();

      return dicom;
#endif  // DICOMPARSER_IGNORE_MAGIC_NUMBER

      }
    }
}


bool DICOMParser::IsValidRepresentation(doublebyte rep, quadbyte& len, VRTypes &mytype)
{
  switch (rep)
    {
    case DICOMParser::VR_AW: 
    case DICOMParser::VR_AE: 
    case DICOMParser::VR_AS: 
    case DICOMParser::VR_CS:
    case DICOMParser::VR_UI:
    case DICOMParser::VR_DA:
    case DICOMParser::VR_DS:
    case DICOMParser::VR_DT:
    case DICOMParser::VR_IS:
    case DICOMParser::VR_LO:
    case DICOMParser::VR_LT:
    case DICOMParser::VR_PN:
    case DICOMParser::VR_ST:
    case DICOMParser::VR_TM:
    case DICOMParser::VR_UT: // new
    case DICOMParser::VR_SH: 
    case DICOMParser::VR_FL:
    case DICOMParser::VR_SL:
    case DICOMParser::VR_AT: 
    case DICOMParser::VR_UL: 
    case DICOMParser::VR_US:
    case DICOMParser::VR_SS:
    case DICOMParser::VR_FD:
      len = DataFile->ReadDoubleByte();
      mytype = VRTypes(rep);
      return true;

    case DICOMParser::VR_OB: // OB - LE
    case DICOMParser::VR_OW:
    case DICOMParser::VR_UN:      
    case DICOMParser::VR_SQ:
      DataFile->ReadDoubleByte();
      len = DataFile->ReadQuadByte();
      mytype = VRTypes(rep);
      return true;

    default:
      //
      //
      // Need to comment out in new paradigm.
      //
      DataFile->Skip(-2);
      len = DataFile->ReadQuadByte();
      mytype = DICOMParser::VR_UNKNOWN;
      return false;  
    }
}

void DICOMParser::ReadNextRecord(doublebyte& group, doublebyte& element)
{
  //
  // WE SHOULD IMPLEMENT THIS ALGORITHM.
  //
  // FIND A WAY TO STOP IF THERE ARE NO MORE CALLBACKS.
  //
  // DO WE NEED TO ENSURE THAT WHEN A CALLBACK IS ADDED THAT 
  // THE IMPLICIT TYPE MAP IS UPDATED?  ONLY IF THERE ISN'T
  // A VALUE IN THE IMPLICIT TYPE MAP.
  //
  // New algorithm:
  //
  // 1. Read group & element
  // 2. ParseExplicitRecord
  //      a. Check to see if the next doublebyte is a valid datatype
  //      b. If the type is valid, lookup type to find the size of
  //              the length field.
  // 3.   If ParseExplicitRecord fails, ParseImplicitRecord
  //      a. Lookup implicit datatype
  // 4. Check to see if there is a registered callback for the group,element.
  // 5. If there are callbacks, read the data and call them, otherwise
  //      skip ahead to the next record.
  //
  //

  group = DataFile->ReadDoubleByte();
  element = DataFile->ReadDoubleByte();

  doublebyte representation = DataFile->ReadDoubleByteAsLittleEndian();
  quadbyte length = 0;
  VRTypes mytype = VR_UNKNOWN;
  this->IsValidRepresentation(representation, length, mytype);
  // this->ParseExplicitRecord(group, element, length, mytype);

  DICOMParserMap::iterator iter = 
    Map.find(DICOMMapKey(group,element));

  VRTypes callbackType;

  if (iter != Map.end())
    {
    //
    // Only read the data if there's a registered callback.
    //
    unsigned char* tempdata = (unsigned char*) DataFile->ReadAsciiCharArray(length);

    DICOMMapKey ge = (*iter).first;
    callbackType = VRTypes(((*iter).second.first));
  
    if (callbackType != mytype &&
        mytype != VR_UNKNOWN)
      {
      //
      // mytype is not VR_UNKNOWN if the file is in Explicit format.
      //
      callbackType = mytype;
      }
 
#ifdef DEBUG_DICOM
    this->DumpTag(this->ParserOutputFile, group, element, callbackType, tempdata, length);
#endif

    std::pair<const DICOMMapKey,DICOMMapValue> p = *iter;
    DICOMMapValue mv = p.second;
    char platformEndian = this->DataFile->GetPlatformEndian()[0];

    bool doSwap;
    /*
    if (platformEndian == 'L')
      {
      doSwap = (this->ToggleByteSwapImageData ^ this->DataFile->GetByteSwap()) && callbackType == VR_OW;
      }
    else
      {
      doSwap = !(doSwap =  (this->ToggleByteSwapImageData ^ this->DataFile->GetByteSwap()) && callbackType == VR_OW);
      }
    */
    doSwap = (this->ToggleByteSwapImageData ^ this->DataFile->GetByteSwap()) && callbackType == VR_OW;

    if (group == 0x7FE0 &&
        element == 0x0010 )
      {
      // if ( (this->ToggleByteSwapImageData ^ this->DataFile->GetByteSwap()) 
      //     && callbackType == VR_OW)
      if (doSwap)
        {
        std::cout << "==============================" << std::endl;
        std::cout << "TOGGLE BS FOR IMAGE" << std::endl;
        std::cout << " ToggleByteSwapImageData : " << this->ToggleByteSwapImageData << std::endl;
        std::cout << " DataFile Byte Swap : " << this->DataFile->GetByteSwap() << std::endl;
        std::cout << "==============================" << std::endl;
        DICOMFile::swapShorts((ushort*) tempdata, (ushort*) tempdata, length/sizeof(ushort));
        }
      else
        {
        std::cout << "==============================" << std::endl;
        std::cout << " AT IMAGE DATA " << std::endl;
        std::cout << " ToggleByteSwapImageData : " << this->ToggleByteSwapImageData << std::endl;
        std::cout << " DataFile Byte Swap : " << this->DataFile->GetByteSwap() << std::endl;

        int t2 = int((0x0000FF00 & callbackType) >> 8);
        int t1 = int((0x000000FF & callbackType));

        if (t1 == 0 && t2 == 0)
          {
          t1 = '?';
          t2 = '?';
          }

        char ct2(t2);
        char ct1(t1);

        std::cout << " Callback type : " << ct1 << ct2 << std::endl;

        std::cout << "==============================" << std::endl;
        }
      }
    else
      {
      if (this->DataFile->GetByteSwap() == true)  
        { 
        switch (callbackType)
          {
          case DICOMParser::VR_OW:
          case DICOMParser::VR_US:
          case DICOMParser::VR_SS:
            DICOMFile::swapShorts((ushort*) tempdata, (ushort*) tempdata, length/sizeof(ushort));
            // std::cout << "16 bit byte swap needed!" << std::endl;
            break;
          case DICOMParser::VR_FL:
          case DICOMParser::VR_FD:
            // std::cout << "Float byte swap needed!" << std::endl;
            /*
            if (this->DataFile->GetByteSwap())
              {
              DICOMFile::swapShorts((ushort*) tempdata, (ushort*) tempdata, length/sizeof(ushort));
              }
            */
            break;
          case DICOMParser::VR_SL:
          case DICOMParser::VR_UL:
            DICOMFile::swapLongs((ulong*) tempdata, (ulong*) tempdata, length/sizeof(ulong));
            // std::cout << "32 bit byte swap needed!" << std::endl;
            break;
          case DICOMParser::VR_AT:
            // std::cout << "ATTRIBUTE Byte swap needed!" << std::endl;
            break;
          default:
            break;
          }
        }
      }

    std::vector<DICOMCallback*> * cbVector = mv.second;
    for (std::vector<DICOMCallback*>::iterator iter = cbVector->begin();
         iter != cbVector->end();
         iter++)
      {
      (*iter)->Execute(ge.first,  // group
                       ge.second,  // element
                       callbackType,  // type
                       tempdata, // data
                       length);  // length
      }

    }
  else
    {
    //
    // Some lengths are negative, but we don't 
    // want to back up the file pointer.
    //
    if (length > 0) 
      {
      DataFile->Skip(length);
      }
#ifdef DEBUG_DICOM
    this->DumpTag(this->ParserOutputFile, group, element, mytype, (unsigned char*) "Unread.", length);
#endif
  }


}

void DICOMParser::InitTypeMap()
{
  DicomRecord dicom_tags[] = {{0x0002, 0x0002, DICOMParser::VR_UI}, // Media storage SOP class uid
                              {0x0002, 0x0003, DICOMParser::VR_UI}, // Media storage SOP inst uid
                              {0x0002, 0x0010, DICOMParser::VR_UI}, // Transfer syntax uid
                              {0x0002, 0x0012, DICOMParser::VR_UI}, // Implementation class uid
                              {0x0008, 0x0018, DICOMParser::VR_UI}, // Image UID
                              {0x0008, 0x0020, DICOMParser::VR_DA}, // Series date
                              {0x0008, 0x0030, DICOMParser::VR_TM}, // Series time
                              {0x0008, 0x0060, DICOMParser::VR_SH}, // Modality
                              {0x0008, 0x0070, DICOMParser::VR_SH}, // Manufacturer
                              {0x0008, 0x1060, DICOMParser::VR_SH}, // Physician
                              {0x0018, 0x0050, DICOMParser::VR_FL}, // slice thickness
                              {0x0018, 0x0060, DICOMParser::VR_FL}, // kV
                              {0x0018, 0x0088, DICOMParser::VR_FL}, // slice spacing
                              {0x0018, 0x1100, DICOMParser::VR_SH}, // Recon diameter
                              {0x0018, 0x1151, DICOMParser::VR_FL}, // mA
                              {0x0018, 0x1210, DICOMParser::VR_SH}, // Recon kernel
                              {0x0020, 0x000d, DICOMParser::VR_UI}, // Study UID
                              {0x0020, 0x000e, DICOMParser::VR_UI}, // Series UID
                              {0x0020, 0x0013, DICOMParser::VR_IS}, // Image number
                              {0x0020, 0x0032, DICOMParser::VR_SH}, // Patient position
                              {0x0020, 0x0037, DICOMParser::VR_SH}, // Patient position cosines
                              {0x0028, 0x0010, DICOMParser::VR_US}, // Num rows
                              {0x0028, 0x0011, DICOMParser::VR_US}, // Num cols
                              {0x0028, 0x0030, DICOMParser::VR_FL}, // pixel spacing
                              {0x0028, 0x0100, DICOMParser::VR_US}, // Bits allocated
                              {0x0028, 0x0120, DICOMParser::VR_UL}, // pixel padding
                              {0x0028, 0x1052, DICOMParser::VR_FL}, // pixel offset
                              {0x7FE0, 0x0010, DICOMParser::VR_OW}   // pixel data
  };


  int num_tags = sizeof(dicom_tags)/sizeof(DicomRecord);

  doublebyte group;
  doublebyte element;
  VRTypes datatype;

  std::vector<DICOMCallback*>* callbackVector;

  for (int i = 0; i < num_tags; i++)
    {
    callbackVector = new std::vector<DICOMCallback*>;
    // callbackVector->push_back(printCb);

    group = dicom_tags[i].group;
    element = dicom_tags[i].element;
    datatype = (VRTypes) dicom_tags[i].datatype;
    // this->SetDICOMTagCallbacks(group, element, datatype, callbackVector);

    TypeMap.insert(std::pair<DICOMMapKey, DICOMTypeValue>(DICOMMapKey(group, element), datatype));
    }
}

void DICOMParser::SetDICOMTagCallbacks(doublebyte group, doublebyte element, VRTypes datatype, std::vector<DICOMCallback*>* cbVector)
{
  Map.insert(std::pair<DICOMMapKey, DICOMMapValue>(DICOMMapKey(group, element), DICOMMapValue((int)datatype, cbVector)));
}


bool DICOMParser::CheckMagic(char* magic_number) 
{
  return (
          (magic_number[0] == DICOM_MAGIC[0]) &&
          (magic_number[1] == DICOM_MAGIC[1]) &&
          (magic_number[2] == DICOM_MAGIC[2]) &&
          (magic_number[3] == DICOM_MAGIC[3])
          );
}

void DICOMParser::DumpTag(std::ostream& out, doublebyte group, doublebyte element, VRTypes vrtype, unsigned char* tempdata, quadbyte length)
{

  int t2 = int((0x0000FF00 & vrtype) >> 8);
  int t1 = int((0x000000FF & vrtype));

  if (t1 == 0 && t2 == 0)
    {
    t1 = '?';
    t2 = '?';
    }

  char ct2(t2);
  char ct1(t1);
    
  out << "(0x";

  out.width(4);
  char prev = out.fill('0');

  out << std::hex << group;
  out << ",0x";

  out.width(4);
  out.fill('0');
    
  out << std::hex << element;
  out << ") ";

  out.fill(prev);
  out << std::dec;
  out << " " << ct1 << ct2 << " ";
  out << "[" << length << " bytes] ";
  
  if (group == 0x7FE0 && element == 0x0010)
    {
    out << "Image data not printed." ;
    }
  else
    {
    out << (tempdata ? (char*) tempdata : "NULL");
    }
 
  out << std::dec << std::endl;
  out.fill(prev);
  out << std::dec;

  return;

}

void DICOMParser::ModalityTag(doublebyte, doublebyte, VRTypes, unsigned char* tempdata, quadbyte)
{
  if (!strcmp( (char*)tempdata, "MR"))
    {
    // this->AddMRTags();
    }
  else if (!strcmp((char*) tempdata, "CT"))
    {
    }
  else if (!strcmp((char*) tempdata, "US"))
    {
    }
}

void DICOMParser::AddMRTags()
{
  DICOMMemberCallback<DICOMParser>* modalityCb = new DICOMMemberCallback<DICOMParser>;
  //modalityCb->SetCallbackFunction(this, &DICOMParser::DumpTag);
    
  std::vector<DICOMCallback*>* modalityCbVector = new std::vector<DICOMCallback*>;
  modalityCbVector->push_back(modalityCb);

  this->SetDICOMTagCallbacks(0x0018, 0x0081, VR_FL, modalityCbVector);
}

void DICOMParser::AddDICOMTagCallbacks(doublebyte group, doublebyte element, VRTypes datatype, std::vector<DICOMCallback*>* cbVector)
{
  DICOMParserMap::iterator miter = Map.find(DICOMMapKey(group,element));
  if (miter != Map.end())
    {
    for (std::vector<DICOMCallback*>::iterator iter = cbVector->begin();
         iter != cbVector->end();
         iter++)
      {
      std::vector<DICOMCallback*>* callbacks = (*miter).second.second;
      callbacks->push_back(*iter);
      }
    }
  else
    {
    this->SetDICOMTagCallbacks(group, element, datatype, cbVector);
    }
}

void DICOMParser::AddDICOMTagCallback(doublebyte group, doublebyte element, VRTypes datatype, DICOMCallback* cb)
{
  DICOMParserMap::iterator miter = Map.find(DICOMMapKey(group,element));
  if (miter != Map.end())
    {
    std::vector<DICOMCallback*>* callbacks = (*miter).second.second;
    callbacks->push_back(cb);
    }
  else
    {
    std::vector<DICOMCallback*>* callback = new std::vector<DICOMCallback*>;
    callback->push_back(cb);
    this->SetDICOMTagCallbacks(group, element, datatype, callback);
    }
}

void DICOMParser::AddDICOMTagCallbackToAllTags(DICOMCallback* cb)
{
  DICOMParserMap::iterator miter;
  for (miter = Map.begin();
       miter != Map.end();
       miter++);
  {
  std::vector<DICOMCallback*>* callbacks = (*miter).second.second;
  callbacks->push_back(cb);
  }
}

bool DICOMParser::ParseExplicitRecord(doublebyte, doublebyte, 
                                      quadbyte& length, 
                                      VRTypes& represent)
{
  doublebyte representation = DataFile->ReadDoubleByte();

  bool valid = this->IsValidRepresentation(representation, length, represent);

  if (valid)
    {
    return true;        
    }
  else
    {
    represent = VR_UNKNOWN;
    length = 0;
    return false;
    }
}

bool DICOMParser::ParseImplicitRecord(doublebyte group, doublebyte element,
                                      quadbyte& length,
                                      VRTypes& represent)
{
  DICOMImplicitTypeMap::iterator iter = 
    TypeMap.find(DICOMMapKey(group,element));
  represent = VRTypes((*iter).second);
  //
  // length?
  //
  length = DataFile->ReadQuadByte();
  return false;
}



void DICOMParser::TransferSyntaxCallback(doublebyte,
                                            doublebyte,
                                            DICOMParser::VRTypes,
                                            unsigned char* val,
                                            quadbyte) 

{
  std::cout << "DICOMParser::TransferSyntaxCallback" << std::endl;

  const char* TRANSFER_UID_EXPLICIT_BIG_ENDIAN = "1.2.840.10008.1.2.2";
  const char* TRANSFER_UID_GE_PRIVATE_IMPLICIT_BIG_ENDIAN = "1.2.840.113619.5.2";

  char* fileEndian = "LittleEndian";
  char* dataEndian = "LittleEndian";

  this->ToggleByteSwapImageData = false;

  if (strcmp(TRANSFER_UID_EXPLICIT_BIG_ENDIAN, (char*) val) == 0)
    {
    std::cout << "EXPLICIT BIG ENDIAN" << std::endl;
    this->ToggleByteSwapImageData = true;
    //
    // Data byte order is big endian
    // 
    // We're always reading little endian in the beginning,
    // so now we need to swap.
    }
  else if (strcmp(TRANSFER_UID_GE_PRIVATE_IMPLICIT_BIG_ENDIAN, (char*) val) == 0)
    {
    std::cout << "GE PRIVATE TRANSFER SYNTAX" << std::endl;
    this->ToggleByteSwapImageData = true;
    std::cout << "ToggleByteSwapImageData : " << this->ToggleByteSwapImageData << std::endl;
    }
  else
    {
    }
}



