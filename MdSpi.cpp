#include "MdSpi.h"
#include "common.h"


void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	LOG(INFO) << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
	LOG(INFO) << "--->>> " << "OnFrontDisconnected" << endl;
	LOG(INFO) << "--->>> Reason = " << nReason << endl;
}
		
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
	LOG(INFO) << "--->>> " << "OnHeartBeatWarning" << endl;
	LOG(INFO) << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	LOG(INFO) << "--->>> " << "OnFrontConnected" << endl;
	///�û���¼����
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	//CThostFtdcReqUserLoginField req;
	//memset(&req, 0, sizeof(req));
	////strcpy(req.BrokerID, BROKER_ID);
	////strcpy(req.UserID, INVESTOR_ID);
	////strcpy(req.Password, PASSWORD);

	int iResult = pMDUserApi->ReqUserLogin(&reqLoginField, ++iRequestID);
	cerr << "--->>> ReqUserLogin: " << ((iResult == 0) ? "Success" : "Fail") << endl;
	LOG(INFO) << "--->>> ReqUserLogin: " << ((iResult == 0) ? "Success" : "Fail") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	LOG(INFO) << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///��ȡ��ǰ������
		cerr << "--->>> GetTradingDay = " << pMDUserApi->GetTradingDay() << endl;

		LOG(INFO) << "--->>> GetTradingDay = " << pMDUserApi->GetTradingDay() << endl;		
		// ����������
		SubscribeMarketData();	
		// ������ѯ��,ֻ�ܶ���֣������ѯ�ۣ�����������ͨ��traderapi��Ӧ�ӿڷ���
		SubscribeForQuoteRsp();	
	}
}

void CMdSpi::SubscribeMarketData()
{
	
	////��ȡ�����ļ�����
	////std::vector <string> list;

	//ifstream inf;

	//inf.open("instruments.txt");


	//string s;

	//while (getline(inf, s))
	//{
	//	//std::vector <string> vect;

	//	//split(s, ",", vect);

	//	//instrumap.insert(map<string, string>::value_type(vect[0], vect[1]));

	//	//string sss = vect[0];

	//	list.push_back(s);

	//}

	//inf.close();

	unsigned int len = InstrumentMap.size();
	//unsigned int len = Instrumap.count;
	char** pInstId = new char*[len];

	//for (unsigned int i = 0; i < len; i++)
	//{
	//	pInstId[i] = (char*)(Instrumentlist[i].first).data();
	//}
	int i = 0;
	for (auto &pair : InstrumentMap) {
		pInstId[i] = (char*)(pair.first).data();
		i++;
	}
	
	//pInstId[i] = (char*)Instrumap[i].data();
	int iResult = pMDUserApi->SubscribeMarketData(pInstId, len);
	
	cerr << "--->>> SubscribeMarketData: " << ((iResult == 0) ? "Success" : "Fail") << endl;
	LOG(INFO) << "--->>> SubscribeMarketData: " << ((iResult == 0) ? "Success" : "Fail") << endl;
	
}

void CMdSpi::SubscribeForQuoteRsp()
{
	unsigned int len = InstrumentMap.size();
	//unsigned int len = Instrumap.count;
	char** pInstId = new char*[len];

	//for (unsigned int i = 0; i < len; i++)
	//	pInstId[i] = (char*)(Instrumentlist[i].first).data();
	
	int i = 0;
	for (auto &pair : InstrumentMap) {
		pInstId[i] = (char*)(pair.first).data();
		i++;
	}
	//pInstId[i] = (char*)Instrumap[i].data();
	int iResult = pMDUserApi->SubscribeForQuoteRsp(pInstId, len);
	
	cerr << "--->>> SubscribeForQuoteRsp: " << ((iResult == 0) ? "Success" : "Fail") << endl;
	LOG(WARNING) << "--->>> SubscribeForQuoteRsp: " << ((iResult == 0) ? "Success" : "Fail") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
  cerr << "[OnRspSubMarketData]|<" << pSpecificInstrument ->InstrumentID<< ">" << endl;
  LOG(INFO)  << "[OnRspSubMarketData]|<" << pSpecificInstrument->InstrumentID << ">" << endl;

}

void CMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "[OnRspSubForQuoteRsp]|<" << pSpecificInstrument->InstrumentID << ">" << endl;
	LOG(WARNING) << "[OnRspSubForQuoteRsp]|<" << pSpecificInstrument->InstrumentID << ">" << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubForQuoteRsp" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{	
  
	//�������	
	   //g_lockqueue.lock();
	 
	
	 CThostFtdcDepthMarketDataField ThostFtdcDepthMarketDataField;
	 
	 ThostFtdcDepthMarketDataField = *pDepthMarketData;
	
	 //MarketDataField.insert(map<string, CThostFtdcDepthMarketDataField>::value_type(pDepthMarketData->InstrumentID, ThostFtdcDepthMarketDataField));
	 
	 MarketDataField[pDepthMarketData->InstrumentID]=ThostFtdcDepthMarketDataField;

	 //if(MarketDataField.second)
	 //{
		// Cout << ��Insert Successfully�� << endl;
	 //}
	 //Else
	 //{
		// Cout << ��Insert Failure�� << endl;
	 //}
	 //int size = m_dataList.size();
	   //g_lockqueue.unlock();
		
		//g_lockqueue.lock();
		//Redis_dataList.push(*pDepthMarketData);
		//int Redis_size = Redis_dataList.size();
		//g_lockqueue.unlock();	
	 
	 ////�򱨼�
	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "ALLNOT")
		// && (Ask[pDepthMarketData->InstrumentID] <= pDepthMarketData->BidPrice1))
	 //{		

		// InstrumentStatus[pDepthMarketData->InstrumentID]="ASK";

	 //}

	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "BID") && (InstrumentStatus[pDepthMarketData->InstrumentID] == "ASKChange")
		// && (Ask[pDepthMarketData->InstrumentID] <= pDepthMarketData->BidPrice1))
	 //{
		// payoff[pDepthMarketData->InstrumentID] += Ask[pDepthMarketData->InstrumentID] - Bid[pDepthMarketData->InstrumentID];
	

		// InstrumentStatus[pDepthMarketData->InstrumentID] = "NONE";

		// LOG(INFO) << "Payoff<" << pDepthMarketData->InstrumentID << "> | "<<payoff[pDepthMarketData->InstrumentID] << endl;

	 //}
	 ////�򱨼�
	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "ALLNOT")
		// && (Bid[pDepthMarketData->InstrumentID] >= pDepthMarketData->AskPrice1))
	 //{

		// InstrumentStatus[pDepthMarketData->InstrumentID] = "BID";

	 //}
	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "ASK") && (InstrumentStatus[pDepthMarketData->InstrumentID] == "BIDChange")
		// && (Bid[pDepthMarketData->InstrumentID] >= pDepthMarketData->AskPrice1))
	 //{
		// payoff[pDepthMarketData->InstrumentID] += Ask[pDepthMarketData->InstrumentID] - Bid[pDepthMarketData->InstrumentID];

		// LOG(INFO) << "Payoff<" << pDepthMarketData->InstrumentID << "> | " << payoff[pDepthMarketData->InstrumentID] << endl;
		// 
		// InstrumentStatus[pDepthMarketData->InstrumentID] = "NONE";
	 //}	 
	// cerr << "Inst:" << pDepthMarketData->InstrumentID<<" | TIME:" << pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec
	//<< " | LastPrice:" << pDepthMarketData->LastPrice << " | Bid:" << pDepthMarketData->BidPrice1
	//	<< " | Volume:" << pDepthMarketData->BidVolume1 << "" << " | Ask:" << pDepthMarketData->AskPrice1
	//	<< " | Volume:" << pDepthMarketData->AskVolume1 << endl;

	//
	//LOG(INFO) << "Inst:" << pDepthMarketData->InstrumentID <<  " | TIME:" << pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec << endl;
	//LOG(INFO) << " | LastPrice:" << pDepthMarketData->LastPrice << " | Bid:" << pDepthMarketData->BidPrice1
	//	<< " | Volume:" << pDepthMarketData->BidVolume1 << "" << " | Ask:" << pDepthMarketData->AskPrice1
	//	<< " | Volume:" << pDepthMarketData->AskVolume1 << endl; 

}

void CMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	
	///ѯ�����������
	g_lockqueue.lock();
	ForQuoteRspField.push(*pForQuoteRsp);
	g_lockqueue.unlock();
	
	cerr      << "[OnRtnForQuoteRsp]" << "Inst:" << pForQuoteRsp->InstrumentID << "|TIME:" << pForQuoteRsp->ForQuoteTime << "|ForQuoteSysID:" << pForQuoteRsp->ForQuoteSysID
		<< "|ExchangeID:" << pForQuoteRsp->ExchangeID<< endl;	
	
	LOG(WARNING) << "[OnRtnForQuoteRsp]" << "Inst:" << pForQuoteRsp->InstrumentID << "|TIME:" << pForQuoteRsp->ForQuoteTime << "|ForQuoteSysID:" << pForQuoteRsp->ForQuoteSysID
		<< "|ExchangeID:" << pForQuoteRsp->ExchangeID << endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr <<    "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	    LOG(INFO)<<"--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	
	return bResult;
}