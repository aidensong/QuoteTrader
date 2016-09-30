#pragma warning(disable : 4996)
#include "TraderSpi.h"
#include <windows.h>
#include <iostream>
#include "common.h"
#include "Config.h"
using namespace std;

TThostFtdcOrderRefType	ORDER_REF;	//��������
TThostFtdcOrderRefType	EXECORDER_REF;	//ִ����������
TThostFtdcOrderRefType	FORQUOTE_REF;	//ѯ������
TThostFtdcOrderRefType	QUOTE_REF;	//��������

// �����ж�
bool IsFlowControl(int iResult)
{
	return ((iResult == -2) || (iResult == -3));
}

void CTraderSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	
	///�ͻ�����֤����
	//ReqAuthenticate();
	ReqUserLogin();
}

///CTP��֤
void CTraderSpi::ReqAuthenticate()
{
	//CThostFtdcReqAuthenticateField  Authenticate;
	//memset(&req, 0, sizeof(req));
	//strcpy(Authenticate.BrokerID, BROKER_ID);
	//strcpy(Authenticate.UserID, INVESTOR_ID);
	//strcpy(Authenticate.UserProductInfo\, PASSWORD);	
	//int iResult = pTraderUserApi->ReqAuthenticate(&Authenticate, ++iRequestID);

	//cerr << "--->>> �����û���¼��֤����: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}
//CTP��֤�ص�����
void CTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	/*cerr << "OnRspAuthenticate" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		
		cerr << "<��֤>||�ͻ�����֤�ɹ���" << endl;
		ReqUserLogin();
	}*/
	
}

void CTraderSpi::ReqUserLogin()
{
	//CThostFtdcReqUserLoginField req;
	//memset(&req, 0, sizeof(req));
	//strcpy(req.BrokerID, BROKER_ID);
	//strcpy(req.UserID, INVESTOR_ID);
	//strcpy(req.Password, PASSWORD);
	int iResult = pTraderUserApi->ReqUserLogin(&reqLoginField, ++iRequestID);
	cerr << "--->>> ReqUserLogin:" << iResult << ((iResult == 0) ? ", Success" : ",Fail") << endl;
}



void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		
		cerr << "<Login>||Login successful" << endl;
		// ����Ự����
		FRONT_ID = pRspUserLogin->FrontID;
		SESSION_ID = pRspUserLogin->SessionID;
	    iNextOrderRef = atoi(pRspUserLogin->MaxOrderRef);
		iNextOrderRef++;
		sprintf(ORDER_REF, "%d", iNextOrderRef);
		
		sprintf(EXECORDER_REF, "%d", 1);
		
		sprintf(FORQUOTE_REF, "%d", 1);
		
		sprintf(QUOTE_REF, "%d", 1);
		///��ȡ��ǰ������
		cerr << "--->>>GetTradingDay=" << pTraderUserApi->GetTradingDay() << endl;
		///Ͷ���߽�����ȷ��
		ReqSettlementInfoConfirm();
	}
}

void CTraderSpi::ReqSettlementInfoConfirm()
{
	CThostFtdcSettlementInfoConfirmField req;
	
	memset(&req, 0, sizeof(req));

	strcpy(req.BrokerID, reqLoginField.BrokerID);
	
	strcpy(req.InvestorID, reqLoginField.UserID);
	
	int iResult = pTraderUserApi->ReqSettlementInfoConfirm(&req, ++iRequestID);
	
	cerr << "--->>> ReqSettlementInfoConfirm: " << iResult << ((iResult == 0) ? ",Success":",Fail") << endl;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspSettlementInfoConfirm" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///�����ѯ��Լ
		ReqQryTradingAccount();
	}
}

void CTraderSpi::ReqQryInstrument()
{
	//CThostFtdcQryInstrumentField req;
	//memset(&req, 0, sizeof(req));
	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	//while (true)
	//{
	//	int iResult = pTraderUserApi->ReqQryInstrument(&req, ++iRequestID);
	//	if (!IsFlowControl(iResult))
	//	{
	//		cerr << "--->>> �����ѯ��Լ: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
	//		break;
	//	}
	//	else
	//	{
	//		cerr << "--->>> �����ѯ��Լ: "  << iResult << ", �ܵ�����" << endl;
	//		Sleep(1000);
	//	}
	//} // while
}

void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryInstrument" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///�����ѯ��Լ
		//ReqQryTradingAccount();
	}
}

void CTraderSpi::ReqQryTradingAccount()
{
	CThostFtdcQryTradingAccountField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryTradingAccount(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> ReqQryTradingAccount:" << iResult << ((iResult == 0) ? ",Success":",Fail") << endl;
			break;
		}
		else
		{
			cerr << "--->>> ReqQryTradingAccount: "  << iResult << ", FlowControl" << endl;
			Sleep(1000);
		}
	} // while
}

void CTraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspQryTradingAccount" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		//�����ѯͶ����ί��
		//ReqQryInvestorPosition();		
		 ReqQryOrder();
	}
}

void CTraderSpi::ReqQryOrder()
{
	CThostFtdcQryOrderField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryOrder(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> ReqQryOrder:" << iResult << ((iResult == 0) ? ",Success" : ",Fail") << endl;
			break;
		}
		else
		{
			cerr << "--->>> ReqQryOrder: " << iResult << ", FlowControl" << endl;
			Sleep(1000);
		}
	} // while
}
//��ѯί��
void CTraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr << "--->>> " << "OnRspQryOrder" << endl;
	if (!IsErrorRspInfo(pRspInfo))
	{  
		if (pOrder!=nullptr)
		{//����ί���б�
			g_lockqueue.lock();			
			//����ί��
			//if ((FRONT_ID == pOrder->FrontID) && SESSION_ID == pOrder->SessionID) 
			//{
				OrderMap[pOrder->OrderRef] = *pOrder;				
				cerr << "[Order]|OrderSysID:" << pOrder->OrderSysID << "|<" << pOrder->InstrumentID
					<< ">|(Bid 0 Ask 1):" << pOrder->Direction 
					<< "|VolumeTotalOriginal:" << pOrder->VolumeTotalOriginal
					<< "|VolumeTotal:" << pOrder->VolumeTotal
					<< "|VolumeTraded:" << pOrder->VolumeTraded					
					<< "|LimitPrice:" << pOrder->LimitPrice
					<< "|OrderStatus:" << pOrder->OrderStatus
					<< endl;
				LOG(INFO)<< "Order|OrderSysID:" << pOrder->OrderSysID << "|<" << pOrder->InstrumentID
					<< ">|(Bid 0 Ask 1):" << pOrder->Direction
					<< "|VolumeTotalOriginal:" << pOrder->VolumeTotalOriginal
					<< "|VolumeTotal:" << pOrder->VolumeTotal
					<< "|VolumeTraded:" << pOrder->VolumeTraded
					<< "|LimitPrice:" << pOrder->LimitPrice
					<< "|OrderStatus:" << pOrder->OrderStatus
					<< endl;
			//}
			g_lockqueue.unlock(); 
		}		
		if (bIsLast)
		{        
			LOG(INFO) << "--------------------------------------------------------------------------------" << endl;
			//�����ѯͶ���ֲ߳�
			ReqQryInvestorPosition();		

		}
	}
	
}

void CTraderSpi::ReqQryInvestorPosition()
{
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);

	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	while (true)
	{
		int iResult = pTraderUserApi->ReqQryInvestorPosition(&req, ++iRequestID);
		if (!IsFlowControl(iResult))
		{
			cerr << "--->>> ReqQryInvestorPosition: " << iResult << ((iResult == 0) ? ",Success" : ",Fail") << endl;
			break;
		}
		else
		{
			cerr << "--->>> ReqQryInvestorPosition: "  << iResult << ", FlowControl" << endl;
			Sleep(1000);
		}
	} // while
}

void CTraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//cerr << "--->>> " << "OnRspQryInvestorPosition" << endl;
	if (!IsErrorRspInfo(pRspInfo))
	{
		if (pInvestorPosition != nullptr)
		{
		//���³ֲ��б�
		g_lockqueue.lock();		
		//����ί�� CTP ����ֲֳ�Ϊ0������ֲֲ�Ϊ0���������Ҫ����һ�¡�		
		if (pInvestorPosition->Position > 0)
		{
			/*	pInvestorPosition->LongFrozen = 0;
				pInvestorPosition->ShortFrozen = 0;
				}*/
			InvestorPositionList.push_back(*pInvestorPosition);			
			cerr << "[Position]|<" << pInvestorPosition->InstrumentID << ">|(2.Long 3.Short):" << pInvestorPosition->PosiDirection << "|Position:" << pInvestorPosition->Position
				<< "|LongFrozen:" << pInvestorPosition->LongFrozen << "|ShortFrozen:" << pInvestorPosition->ShortFrozen << endl;
			LOG(INFO) << "[Position]|<" << pInvestorPosition->InstrumentID << ">|(2.Long 3.Short):" << pInvestorPosition->PosiDirection << "|Position:" << pInvestorPosition->Position
				<< "|LongFrozen��" << pInvestorPosition->LongFrozen << "|ShortFrozen��" << pInvestorPosition->ShortFrozen << endl;
		}
		g_lockqueue.unlock();
	    }
	}
	///
	if (bIsLast)
	{
		LOG(INFO) << "--------------------------------------------------------------------------------" << endl;
		/////��ʼ�����
		//std::unique_lock <std::mutex> lck(g_lockqueue);
		InitFinished = true; // ����ȫ�ֱ�־λΪ true.
		//cv.notify_all(); // ���������߳�.		
	}
}

void CTraderSpi::ReqOrderInsert(TThostFtdcDirectionType DIRECTION, TThostFtdcPriceType LIMIT_PRICE, string InstrumentID,int volume)
{
	
		CThostFtdcInputOrderField req;		
		
		memset(&req, 0, sizeof(req));
		strcpy(req.BrokerID, reqLoginField.BrokerID);
		strcpy(req.InvestorID, reqLoginField.UserID);
		///��Լ����
		strcpy(req.InstrumentID, (char*)InstrumentID.data());
						
		iNextOrderRef++;
						
		sprintf(ORDER_REF, "%d", iNextOrderRef);
						
		///��������
		strcpy(req.OrderRef, ORDER_REF);				
						
		///�û�����
		//	TThostFtdcUserIDType	UserID;
		///�����۸�����: �޼�
		req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
		///��������: 
		req.Direction = DIRECTION;
		int volumn = CheckEnClose(InstrumentID, DIRECTION);

		if (CheckEnClose(InstrumentID, DIRECTION) > 0)
		{
			///��Ͽ�ƽ��־: ƽ��
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;//THOST_FTDC_OF_Open;
		}
		else
			//��Ͽ�ƽ��־: ����
			req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
						
						
		///���Ͷ���ױ���־
		req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
						
		///�۸�
		req.LimitPrice = LIMIT_PRICE;
						
		///����: 1
		req.VolumeTotalOriginal = volume;
		///��Ч������: ������Ч
		req.TimeCondition = THOST_FTDC_TC_GFD;
		///GTD����
		//	TThostFtdcDateType	GTDDate;
		///�ɽ�������: �κ�����
		req.VolumeCondition = THOST_FTDC_VC_AV;
		///��С�ɽ���: 1
		req.MinVolume = 1;
		///��������: ����
		req.ContingentCondition = THOST_FTDC_CC_Immediately;
		///ֹ���
		//	TThostFtdcPriceType	StopPrice;
		///ǿƽԭ��: ��ǿƽ
		req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
		///�Զ������־: ��
		req.IsAutoSuspend = 0;
		///ҵ��Ԫ
		//	TThostFtdcBusinessUnitType	BusinessUnit;
		///������
		//	TThostFtdcRequestIDType	RequestID;
		///�û�ǿ����־: ��
		req.UserForceClose = 0;//				
						
		int iResult = pTraderUserApi->ReqOrderInsert(&req, ++iRequestID);						
						
		if (DIRECTION == THOST_FTDC_D_Buy)
		{
			cerr << ">>>>>> ����<" << InstrumentID << ">|�۸�" << LIMIT_PRICE << "¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
			LOG(INFO) << ">>>>>> ����<" << InstrumentID << ">|�۸�" << LIMIT_PRICE << "¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
		}
		else
		{
			cerr << ">>>>>> ����<" << InstrumentID << ">|�۸�" << LIMIT_PRICE << "¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
			LOG(INFO) << ">>>>>> ����<" << InstrumentID << ">|�۸�" << LIMIT_PRICE << "¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
						
		}
}

// ���������Ӧ��AVAILABLE�� 
void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

	//���ͨ��CTP��֤������¼����ȷ���򲻻����ûص���
	cerr << "--->>> " << "OnRspOrderInsert" << endl;

	IsErrorRspInfo(pRspInfo);

	Msg MsgOrderInsert;

	//MsgType Type = RtnTrade;

	MsgOrderInsert.Msg_Type = InputOrder;

	MsgOrderInsert.InputOrder = *pInputOrder;

	MsgOrderInsert.RspInfo = *pRspInfo;

	g_lockqueue.lock();

	MsgQueue.push(MsgOrderInsert);

	g_lockqueue.unlock();

}

void CTraderSpi::ReqOrderAction(CThostFtdcOrderField *pOrder)
{
	//static bool ORDER_ACTION_SENT = false;		//�Ƿ����˱���

	//if (ORDER_ACTION_SENT)
	//	return;

	CThostFtdcInputOrderActionField req;

	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);

	///������������
	//	TThostFtdcOrderActionRefType	OrderActionRef;
	///��������
	strcpy(req.OrderRef, pOrder->OrderRef);
	///������
	//	TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID = FRONT_ID;
	///�Ự���
	req.SessionID = SESSION_ID;

	///����������
	//	TThostFtdcExchangeIDType	ExchangeID;
	///�������
	//	TThostFtdcOrderSysIDType	OrderSysID;

	///������־
	req.ActionFlag = THOST_FTDC_AF_Delete;

	///�۸�
	//	TThostFtdcPriceType	LimitPrice;
	///�����仯
	//	TThostFtdcVolumeType	VolumeChange;
	///�û�����
	//	TThostFtdcUserIDType	UserID;
	///��Լ����

	strcpy(req.InstrumentID, pOrder->InstrumentID);

	int iResult = pTraderUserApi->ReqOrderAction(&req, ++iRequestID);

	cerr     << "<ί�г���>|ί�б��:<" << pOrder->OrderRef << ">|�۸�:" << pOrder->LimitPrice << "|����(0.��1.��):" << pOrder->Direction<< iResult << ((iResult == 0) ? ",�ɹ�" : ",ʧ��") << endl;
	LOG(INFO)<< "<ί�г���>|ί�б��:<" << pOrder->OrderRef << ">|�۸�:" << pOrder->LimitPrice << "|����(0.��1.��):" << pOrder->Direction << iResult << ((iResult == 0) ? ",�ɹ�" : ",ʧ��") << endl;


	//}///ORDER_ACTION_SENT = true;
}

// ���������Ӧ ��AVAILABLE��
void CTraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

	//���ͨ��CTP��֤������������ȷ���򲻻����ûص���
	cerr << "--->>> " << "OnRspOrderAction" << endl;
	
	IsErrorRspInfo(pRspInfo);

	Msg MsgInputOrderAction;	

	MsgInputOrderAction.Msg_Type = InputOrderAction;

	if (pInputOrderAction!=nullptr)
	MsgInputOrderAction.InputOrderAction = *pInputOrderAction;
	
	if (pRspInfo != nullptr)
	MsgInputOrderAction.RspInfo = *pRspInfo;

	g_lockqueue.lock();

	MsgQueue.push(MsgInputOrderAction);

	g_lockqueue.unlock();
}

///  �ɽ�֪ͨ��AVAILABLE�� 
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{


	Msg MsgTrade;

	//MsgType Type = RtnTrade;

	MsgTrade.Msg_Type = RtnTrade;
	if (pTrade!=nullptr)
	MsgTrade.RtnTrade = *pTrade;

	g_lockqueue.lock();

	MsgQueue.push(MsgTrade);

	g_lockqueue.unlock();

	/*if ((pTrade->Direction == THOST_FTDC_D_Buy))
	if (pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
	LongPosition[pTrade->InstrumentID]++;
	LongEnClose[pTrade->InstrumentID]++;
	}
	else
	{
	ShortPosition[pTrade->InstrumentID]--;
	}

	else

	if (pTrade->OffsetFlag == THOST_FTDC_OF_Open)
	{
	ShortPosition[pTrade->InstrumentID]++;
	ShortEnClose[pTrade->InstrumentID]++;
	}
	else
	{
	LongPosition[pTrade->InstrumentID]--;
	}

	if ((pTrade->Direction == THOST_FTDC_D_Buy) && (InstrumentStatus[pTrade->InstrumentID] == "ALLNOT"))
	{
	InstrumentStatus[pTrade->InstrumentID] = "BID";

	cerr << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price<< "" << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;
	}
	if ((pTrade->Direction == THOST_FTDC_D_Sell) && (InstrumentStatus[pTrade->InstrumentID] == "ALLNOT"))
	{

	InstrumentStatus[pTrade->InstrumentID] = "ASK";

	cerr << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;
	}

	if ((pTrade->Direction == THOST_FTDC_D_Buy) && (InstrumentStatus[pTrade->InstrumentID] == "ASK"))
	{
	InstrumentStatus[pTrade->InstrumentID] = "NONE";

	cerr << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	return;
	}
	if ((pTrade->Direction == THOST_FTDC_D_Sell) && (InstrumentStatus[pTrade->InstrumentID] == "ASK_Change"))
	{

	InstrumentStatus[pTrade->InstrumentID] = "NONE";

	cerr << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	return;
	}
	if ((pTrade->Direction == THOST_FTDC_D_Buy) && (InstrumentStatus[pTrade->InstrumentID] == "BID_Change"))
	{

	InstrumentStatus[pTrade->InstrumentID] = "NONE";
	cerr << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;

	}
	if ((pTrade->Direction == THOST_FTDC_D_Sell) && (InstrumentStatus[pTrade->InstrumentID] == "BID"))
	{
	InstrumentStatus[pTrade->InstrumentID] = "NONE";

	cerr << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ��<" << pTrade->InstrumentID << "> | ���� ������ | �ɽ��۸�" << pTrade->Price << "" << endl;

	cerr << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	LOG(INFO) << "OnRtnTrade | ��Լ: <" << pTrade->InstrumentID << "> | Status : " << InstrumentStatus[pTrade->InstrumentID] << endl;

	return;
	}*/

}

///����֪ͨ AVAILABLE
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	//����ί��
	//if ((FRONT_ID == pOrder->FrontID) && (SESSION_ID == pOrder->SessionID))
	//{
		g_lockqueue.lock();

		Msg MsgOrder;

		//MsgType Type = RtnTrade;

		MsgOrder.Msg_Type = RtnOrder;

		MsgOrder.RtnOrder = *pOrder;

		MsgQueue.push(MsgOrder);

		g_lockqueue.unlock();
//	}
	
	//	/////////////////////////////////////////////////////////////////////////
	//	///TFtdcOrderStatusType��һ������״̬����
	//	/////////////////////////////////////////////////////////////////////////
	//	///ȫ���ɽ�
	//#define THOST_FTDC_OST_AllTraded '0'
	//	///���ֳɽ����ڶ�����
	//#define THOST_FTDC_OST_PartTradedQueueing '1'
	//	///���ֳɽ����ڶ�����
	//#define THOST_FTDC_OST_PartTradedNotQueueing '2'
	//	///δ�ɽ����ڶ�����
	//#define THOST_FTDC_OST_NoTradeQueueing '3'
	//	///δ�ɽ����ڶ�����
	//#define THOST_FTDC_OST_NoTradeNotQueueing '4'
	//	///����
	//#define THOST_FTDC_OST_Canceled '5'
	//	///δ֪
	//#define THOST_FTDC_OST_Unknown 'a'
	//	///��δ����
	//#define THOST_FTDC_OST_NotTouched 'b'
	//	///�Ѵ���
	//#define THOST_FTDC_OST_Touched 'c'
	//typedef char TThostFtdcOrderStatusType;	
	//	///�����Ѿ��ύ	
	//cerr << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//string isOpen = "";
	///OrderStatus״̬�޸�Ϊ�ɽ�ǰ�����ٴε��ã����ٴ��޸Ŀ�ƽ�֣�//��OrderStatus״̬�޸�Ϊ�ɽ��޸�Ϊ�ɽ���Ӧ�ý���ƽ������ԭ
	//if (pOrder->OrderStatus == THOST_FTDC_OST_NoTradeQueueing)
	//	//&& (pOrder->OrderSubmitStatus != THOST_FTDC_OSS_Accepted))
	//{
	//	if (pOrder->Direction == THOST_FTDC_D_Buy)
	//	{
	//		BidORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			ShortEnClose[pOrder->InstrumentID]--;
	//			isOpen = "ƽ��";
	//		}
	//		cerr << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		LOG(INFO) << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		cerr << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ���ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ���ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//	if (pOrder->Direction == THOST_FTDC_D_Sell)
	//	{
	//		string isOpen = "";
	//		AskORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			LongEnClose[pOrder->InstrumentID]--;
	//		}
	//		cerr << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		LOG(INFO) << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		cerr << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ��ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ��ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//}
	////�����ɽ������������޸ĵĿ�ƽ����
	//if (pOrder->OrderStatus == THOST_FTDC_OST_AllTraded)
	//	//&& (pOrder->OrderSubmitStatus != THOST_FTDC_OSS_Accepted))
	//{
	//	if (pOrder->Direction == THOST_FTDC_D_Buy)
	//	{
	//		//BidORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			ShortEnClose[pOrder->InstrumentID]++;
	//		}
	//	}
	//	if (pOrder->Direction == THOST_FTDC_D_Sell)
	//	{
	//		//	string isOpen = "";
	//		//AskORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			LongEnClose[pOrder->InstrumentID]++;
	//		}
	//	}
	//}
	//////
	/////�����ύ
	//if (pOrder->OrderStatus == THOST_FTDC_OST_Canceled)
	//{
	//	if (pOrder->Direction == THOST_FTDC_D_Buy)
	//	{
	//		BidORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			ShortEnClose[pOrder->InstrumentID]++;
	//		}
	//		cerr << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		LOG(INFO) << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		cerr << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ���ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ���ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//	if (pOrder->Direction == THOST_FTDC_D_Sell)
	//	{
	//		AskORDER_REF[pOrder->InstrumentID] = pOrder->OrderRef;
	//		if (pOrder->CombOffsetFlag[0] != THOST_FTDC_OF_Open)
	//		{
	//			LongEnClose[pOrder->InstrumentID]++;
	//		}
	//		cerr << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		LOG(INFO) << " OnRtnOrder |��Լ��<" << pOrder->InstrumentID << "> | ���� ��" << isOpen << "����  ����״̬ ���������ύ" << endl;
	//		cerr << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ��ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//		LOG(INFO) << "OnRtnOrder | ��Լ: <" << pOrder->InstrumentID << "> | ��ֿ��� : " << ShortEnClose[pOrder->InstrumentID] << endl;
	//	}
	//}
}
//**********************************************************************************************
//**********************************************************************************************
//ִ������¼������
void CTraderSpi::ReqExecOrderInsert()
{
	//CThostFtdcInputExecOrderField req;
	//memset(&req, 0, sizeof(req));
	/////���͹�˾����
	//strcpy(req.BrokerID, BROKER_ID);
	/////Ͷ���ߴ���
	//strcpy(req.InvestorID, INVESTOR_ID);
	/////��Լ����
	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	/////��������
	//strcpy(req.ExecOrderRef, EXECORDER_REF);
	/////�û�����
	////	TThostFtdcUserIDType	UserID;
	/////����
	//req.Volume=1;
	/////������
	////TThostFtdcRequestIDType	RequestID;
	/////ҵ��Ԫ
	////TThostFtdcBusinessUnitType	BusinessUnit;
	/////��ƽ��־
	//req.OffsetFlag=THOST_FTDC_OF_Close;//���������������Ҫ��ƽ���ƽ��
	/////Ͷ���ױ���־
	//req.HedgeFlag=THOST_FTDC_HF_Speculation;
	/////ִ������
	//req.ActionType=THOST_FTDC_ACTP_Exec;//�������ִ������THOST_FTDC_ACTP_Abandon
	/////����ͷ������ĳֲַ���
	//req.PosiDirection=THOST_FTDC_PD_Long;
	/////��Ȩ��Ȩ���Ƿ����ڻ�ͷ��ı��
	//req.ReservePositionFlag=THOST_FTDC_EOPF_UnReserve;//�����н��������������֣������THOST_FTDC_EOPF_Reserve
	/////��Ȩ��Ȩ�����ɵ�ͷ���Ƿ��Զ�ƽ��
	//req.CloseFlag=THOST_FTDC_EOCF_AutoClose;//�����н��������������֣������THOST_FTDC_EOCF_NotToClose

	//int iResult = pTraderUserApi->ReqExecOrderInsert(&req, ++iRequestID);
	//cerr << "--->>> ִ������¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}
//ѯ��¼������
void CTraderSpi::ReqForQuoteInsert()
{
	//CThostFtdcInputForQuoteField req;
	//memset(&req, 0, sizeof(req));
	/////���͹�˾����
	//strcpy(req.BrokerID, BROKER_ID);
	/////Ͷ���ߴ���
	//strcpy(req.InvestorID, INVESTOR_ID);
	/////��Լ����
	//strcpy(req.InstrumentID, INSTRUMENT_ID);
	/////��������
	//strcpy(req.ForQuoteRef, EXECORDER_REF);
	/////�û�����
	////	TThostFtdcUserIDType	UserID;

	//int iResult = pTraderUserApi->ReqForQuoteInsert(&req, ++iRequestID);
	//cerr << "--->>> ѯ��¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
}
//����¼������
string CTraderSpi::ReqQuoteInsert(TThostFtdcOrderSysIDType ForQuoteSysID, TThostFtdcPriceType LIMIT_PRICE, string InstrumentID, int volume, double spreed)
{
	CThostFtdcInputQuoteField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, reqLoginField.BrokerID);
	strcpy(req.InvestorID, reqLoginField.UserID);
	///��Լ����
	strcpy(req.InstrumentID, (char*)InstrumentID.data());

	iNextOrderRef++;

	sprintf(ORDER_REF, "%d", iNextOrderRef);

	///��������
	strcpy(req.QuoteRef, ORDER_REF);

	///��������
	//strcpy(req.QuoteRef, QUOTE_REF);
	///���۸�
	req.AskPrice = LIMIT_PRICE + spreed;
	///��۸�
	req.BidPrice = LIMIT_PRICE - spreed;
	///������
	req.AskVolume = volume;
	///������
	req.BidVolume = volume;
	///������
	//TThostFtdcRequestIDType	RequestID;
	///ҵ��Ԫ
	//TThostFtdcBusinessUnitType	BusinessUnit;	
	
	///����ƽ��־
	req.AskOffsetFlag=THOST_FTDC_OF_Open;
	///��ƽ��־
	req.BidOffsetFlag=THOST_FTDC_OF_Open;
	///��Ͷ���ױ���־
	req.AskHedgeFlag=THOST_FTDC_HF_Speculation;
	///��Ͷ���ױ���־
	req.BidHedgeFlag=THOST_FTDC_HF_Speculation;
	
	 strcpy(req.ForQuoteSysID, ForQuoteSysID);
	
	 //req.ForQuoteSysID = ForQuoteSysID;
	
	int iResult = pTraderUserApi->ReqQuoteInsert(&req, ++iRequestID);
	cerr << "--->>> ReqQuoteInsert: " << iResult << ((iResult == 0) ? ", Success" : ", Fail") << endl;
	return  ((iResult == 0) ? ORDER_REF : "error");
}
void CTraderSpi::OnRspExecOrderInsert(CThostFtdcInputExecOrderField *pInputExecOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//���ִ��������ȷ���򲻻����ûص�
	cerr << "--->>> " << "OnRspExecOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}
void CTraderSpi::OnRspForQuoteInsert(CThostFtdcInputForQuoteField *pInputForQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//���ѯ����ȷ���򲻻����ûص�
	cerr << "--->>> " << "OnRspForQuoteInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}
void CTraderSpi::OnRspQuoteInsert(CThostFtdcInputQuoteField *pInputQuote, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//���������ȷ���򲻻����ûص�
	cerr << "--->>> " << "OnRspQuoteInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}
void CTraderSpi::ReqExecOrderAction(CThostFtdcExecOrderField *pExecOrder)
{
	//static bool EXECORDER_ACTION_SENT = false;		//�Ƿ����˱���
	//if (EXECORDER_ACTION_SENT)
	//	return;

	//CThostFtdcInputExecOrderActionField req;
	//memset(&req, 0, sizeof(req));

	/////���͹�˾����
	//strcpy(req.BrokerID,pExecOrder->BrokerID);
	/////Ͷ���ߴ���
	//strcpy(req.InvestorID,pExecOrder->InvestorID);
	/////ִ�������������
	////TThostFtdcOrderActionRefType	ExecOrderActionRef;
	/////ִ����������
	//strcpy(req.ExecOrderRef,pExecOrder->ExecOrderRef);
	/////������
	////TThostFtdcRequestIDType	RequestID;
	/////ǰ�ñ��
	//req.FrontID=FRONT_ID;
	/////�Ự���
	//req.SessionID=SESSION_ID;
	/////����������
	////TThostFtdcExchangeIDType	ExchangeID;
	/////ִ������������
	////TThostFtdcExecOrderSysIDType	ExecOrderSysID;
	/////������־
	//req.ActionFlag=THOST_FTDC_AF_Delete;
	/////�û�����
	////TThostFtdcUserIDType	UserID;
	/////��Լ����
	//strcpy(req.InstrumentID,pExecOrder->InstrumentID);

	//int iResult = pTraderUserApi->ReqExecOrderAction(&req, ++iRequestID);
	//cerr << "--->>> ִ�������������: "  << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
	//EXECORDER_ACTION_SENT = true;
}

void CTraderSpi::ReqQuoteAction(CThostFtdcQuoteField *pQuote)
{
	//static bool QUOTE_ACTION_SENT = false;		//�Ƿ����˱���
	//if (QUOTE_ACTION_SENT)
	//	return;

	CThostFtdcInputQuoteActionField req;
	memset(&req, 0, sizeof(req));
	///���͹�˾����
	strcpy(req.BrokerID, pQuote->BrokerID);
	///Ͷ���ߴ���
	strcpy(req.InvestorID, pQuote->InvestorID);
	///���۲�������
	//TThostFtdcOrderActionRefType	QuoteActionRef;
	///��������
	strcpy(req.QuoteRef,pQuote->QuoteRef);
	///������
	//TThostFtdcRequestIDType	RequestID;
	///ǰ�ñ��
	req.FrontID=FRONT_ID;
	///�Ự���
	req.SessionID=SESSION_ID;
	///����������
	//TThostFtdcExchangeIDType	ExchangeID;
	///���۲������
	//TThostFtdcOrderSysIDType	QuoteSysID;
	///������־
	req.ActionFlag=THOST_FTDC_AF_Delete;
	///�û�����
	//TThostFtdcUserIDType	UserID;
	///��Լ����
	strcpy(req.InstrumentID,pQuote->InstrumentID);

	int iResult = pTraderUserApi->ReqQuoteAction(&req, ++iRequestID);
	cerr << "--->>> ReqQuoteAction: " << iResult << ((iResult == 0) ? ",Success" : ",Fail") << endl;
	//QUOTE_ACTION_SENT = true;
}
void CTraderSpi::OnRspExecOrderAction(CThostFtdcInputExecOrderActionField *pInpuExectOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//��ȷ�ĳ����������������ûص�
	cerr << "--->>> " << "OnRspExecOrderAction" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CTraderSpi::OnRspQuoteAction(CThostFtdcInputQuoteActionField *pInpuQuoteAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	//��ȷ�ĳ����������������ûص�
	cerr << "--->>> " << "OnRspQuoteAction" << endl;
	IsErrorRspInfo(pRspInfo);
}
//ִ������֪ͨ
void CTraderSpi::OnRtnExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	//pExecOrder->
	//
	//
	//
	//if (IsMyExecOrder(pExecOrder))
	//{
 //      if (pExecOrder->ExecResult == THOST_FTDC_OER_Canceled)
	//   if (pExecOrder->OffsetFlag != THOST_FTDC_OF_Open)
	//   {
	//	   LongEnClose[pExecOrder->InstrumentID]++;
	//    }
 //    
	//}
	//cerr << "���� | ��Լ: <" << pExecOrder->InstrumentID << "> | ���� : " << InstrumentStatus[pExecOrder->InstrumentID] << endl;
	//LOG(INFO) << "���� | ��Լ: <" << pExecOrder->InstrumentID << "> | ���� : " << InstrumentStatus[pTrade->InstrumentID] << endl;
	//cerr << "--->>> " << "����" << endl;
 }
//ѯ��֪ͨ
void CTraderSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	//�������н���ѯ��֪ͨͨ���ýӿڷ��أ�ֻ�������̿ͻ������յ���֪ͨ
	cerr << "--->>> " << "OnRtnForQuoteRsp"  << endl;
}
//����֪ͨ
void CTraderSpi::OnRtnQuote(CThostFtdcQuoteField *pQuote)
{
	//cerr << "--->>> " << "OnRtnQuote"  << endl;
	g_lockqueue.lock();

	Msg MsgQuote;

	//MsgType Type = RtnTrade;

	MsgQuote.Msg_Type = RtnQuote;

	MsgQuote.RtnQuote = *pQuote;

	MsgQueue.push(MsgQuote);

	g_lockqueue.unlock();
	
	cerr << "[OnRtnQuote]|<" << pQuote->InstrumentID << ">|Bid:" << pQuote->BidPrice 
		<< "|" << pQuote->BidVolume << "|Ask:" << pQuote->AskPrice<<"|" << pQuote->AskVolume<<endl;
	LOG(INFO)<< " [OnRtnQuote]|<" << pQuote->InstrumentID << ">|Bid:" << pQuote->BidPrice
		<< "|" << pQuote->BidVolume << "|Ask:" << pQuote->AskPrice << "|" << pQuote->AskVolume << endl;;
	
	
	//if (IsMyQuote(pQuote))
	//{
	//	if (IsTradingQuote(pQuote))
	//		ReqQuoteAction(pQuote);
	//	else if (pQuote->QuoteStatus == THOST_FTDC_OST_Canceled)
	//		cout << "--->>> ���۳����ɹ�" << endl;
	//}
}

void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void CTraderSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}
/// 
void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}


// ���ErrorID != 0, ˵���յ��˴������Ӧ AVAILABLE
bool CTraderSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// ���ErrorID != 0, ˵���յ��˴������Ӧ

	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
	{
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
		LOG(INFO) << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;

	}
   return bResult;
}

bool CTraderSpi::IsMyOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->FrontID == FRONT_ID) &&
			(pOrder->SessionID == SESSION_ID) &&
			(strcmp(pOrder->OrderRef, ORDER_REF) == 0));
}

///����¼�����ر�
void CTraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo)
{

	cerr << "--->>> " << "OnErrRtnOrderInsert" << endl;
	IsErrorRspInfo(pRspInfo);
}


///������������ر�
void CTraderSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo)
{
	//
}
bool CTraderSpi::IsMyExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return ((pExecOrder->FrontID == FRONT_ID) &&
		(pExecOrder->SessionID == SESSION_ID) &&
		(strcmp(pExecOrder->ExecOrderRef, EXECORDER_REF) == 0));
}

bool CTraderSpi::IsMyQuote(CThostFtdcQuoteField *pQuote)
{
	return ((pQuote->FrontID == FRONT_ID) &&
		(pQuote->SessionID == SESSION_ID) &&
		(strcmp(pQuote->QuoteRef, QUOTE_REF) == 0));
}

bool CTraderSpi::IsTradingOrder(CThostFtdcOrderField *pOrder)
{
	return ((pOrder->OrderStatus != THOST_FTDC_OST_PartTradedNotQueueing) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_Canceled) &&
			(pOrder->OrderStatus != THOST_FTDC_OST_AllTraded));
}

bool CTraderSpi::IsTradingExecOrder(CThostFtdcExecOrderField *pExecOrder)
{
	return (pExecOrder->ExecResult != THOST_FTDC_OER_Canceled);
}

bool CTraderSpi::IsTradingQuote(CThostFtdcQuoteField *pQuote)
{
	return (pQuote->QuoteStatus != THOST_FTDC_OST_Canceled);
}

