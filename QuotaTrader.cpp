#include "QuotaTrader.h"
#include "common.h"
#include "Config.h"

//�ص���
CMdSpi* pMDUserSpi = nullptr;

CTraderSpi* pTraderUserSpi = nullptr;

string MDFRONT_ADDR;       //�����ַ
string TRADERFRONT_ADDR;   //���׵�ַ
string INVESTOR_ID;        //�û�
string PASSWORD;           //����
string BROKERID;           // ���͹�˾����
string StrSpreed;         //�۲�
string StrVolume;         //��������
string strUserProductInfo; //�û��˲�Ʒ��Ϣ
string	strAuthCode; ///��֤��

///��ȡ��Լ�����ļ�
void  SysInit()
{
	const char ConfigFile[] = "config.txt";
	Config configSettings(ConfigFile);

	MDFRONT_ADDR = configSettings.Read("MDAddress", MDFRONT_ADDR);

	TRADERFRONT_ADDR = configSettings.Read("TraderAddress", TRADERFRONT_ADDR);

	BROKERID = configSettings.Read("brokerID", BROKERID);

	INVESTOR_ID = configSettings.Read("user", INVESTOR_ID);

	PASSWORD = configSettings.Read("pwd", PASSWORD);

	strUserProductInfo = configSettings.Read("UserProductInfo", strUserProductInfo);

	strAuthCode = configSettings.Read("AuthCode", strAuthCode);

	StrSpreed = configSettings.Read("spreed", StrSpreed);
	stringstream convert;
	convert << StrSpreed;
	convert >> spreed;

	StrVolume = configSettings.Read("volume", StrVolume);
	stringstream convertvol;
	convertvol << StrVolume;
	convertvol >> volume; 

	//memset(&req, 0, sizeof(req));
	//strcpy(Authenticate.BrokerID, BROKER_ID);
	//strcpy(Authenticate.UserID, INVESTOR_ID);
	//strcpy(Authenticate.UserProductInfo\, PASSWORD);
	//��ȡ�����ļ�����
	//std::vector <string> list;
	ifstream inf;
	inf.open("instruments.csv");
	string s;
	while (getline(inf, s))
	{
		vector<string> listStr = Strsplit(s, ",");	
		
		double SettlementPrice;
		stringstream sstr(listStr[1]);
		sstr >> SettlementPrice;	
		
		InstrumentMap.insert(map<string, double>::value_type(listStr[0], SettlementPrice));
		
		//.push_back(make_pair(listStr[0], SettlementPrice));	
		
		CThostFtdcDepthMarketDataField DepthMarketData;		
		
		DepthMarketData.LastPrice = -1;
		DepthMarketData.AskPrice1 = -1;
		DepthMarketData.BidPrice1 = -1;

		//������Ӧ��map key
		MarketDataField.insert(map<string, CThostFtdcDepthMarketDataField>::value_type(s, DepthMarketData));	
		
		payoff.insert(map<string, int>::value_type(s, 0));
		
		//�򱨼�����
		BidORDER_REF_present.insert(map<string, string>::value_type(s, ""));
		
		//����������
		AskORDER_REF_present.insert(map<string, string>::value_type(s, ""));


		//if (s.length()>6)
		//	s.substr(8, 4);
	}
	inf.close();
}
//��ӡ�ֲ�
void  LogpInvestorPosition()
{
	
	     cerr << "----------------------------Position-----------------------------------"<< endl;
	LOG(INFO) << "----------------------------Position-----------------------------------" << endl;
	
	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	{
		if (InvestorPosition.Position > 0)  //���ڳֲ֣��Ŵ�ӡ�ֲּ�¼
		{
			
			//InvestorPositionList.push_back(*pInvestorPosition);
			cerr << "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long 3.Short):"
				<< InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
			/*	<< "|Frozen:"
				<< (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)*/
				<< "|Enable:"
				<< InvestorPosition.Position - (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)
				<< endl;
			LOG(INFO) << "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long 3.Short):"
				<< InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
			/*	<< "|Frozen:"
				<< (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)*/
				<< "|Enable:"
				<< InvestorPosition.Position - (InvestorPosition.PosiDirection == '2' ? InvestorPosition.ShortFrozen : InvestorPosition.LongFrozen)
				<< endl;
			
	/*		cerr << "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long3.Short):" << InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
				<< "|LongFrozen:" << InvestorPosition.LongFrozen << "|ShortFrozen:" << InvestorPosition.ShortFrozen << endl;
	   LOG(INFO) << "[Position]|<" << InvestorPosition.InstrumentID << ">|(2.Long3.Short):" << InvestorPosition.PosiDirection << "|Position:" << InvestorPosition.Position
				<< "|LongFrozen:" << InvestorPosition.LongFrozen << "|ShortFrozen:" << InvestorPosition.ShortFrozen << endl;*/
		}
	}

       	 cerr << "----------------------------------------------------------------------" << endl;
	LOG(INFO) << "----------------------------------------------------------------------" << endl;
}
///�����߳� ˫�߱��۲���
void QuotaStrategy()
{
	//	/// typedef char TThostFtdcOrderStatusType;  TFtdcOrderStatusType��һ������״̬����
	
	//#define THOST_FTDC_OST_AllTraded '0' ȫ���ɽ� 
	//	///
	//#define THOST_FTDC_OST_PartTradedQueueing '1' ���ֳɽ����ڶ�����
	//	///
	//#define THOST_FTDC_OST_PartTradedNotQueueing '2' ���ֳɽ����ڶ�����
	//	///
	//#define THOST_FTDC_OST_NoTradeQueueing '3' δ�ɽ����ڶ�����
	//	///
	//#define THOST_FTDC_OST_NoTradeNotQueueing '4' δ�ɽ����ڶ�����
	//	///
	//#define THOST_FTDC_OST_Canceled '5' ����
	//	///
	//#define THOST_FTDC_OST_Unknown 'a' δ֪
	//	///
	//#define THOST_FTDC_OST_NotTouched 'b' ��δ����
	//	///
	//#define THOST_FTDC_OST_Touched 'c' �Ѵ���

	//�г�������������	
	for (pair<string,double> InstrumentPair : Instrumentlist)
	{
		
		string InstrumentID = InstrumentPair.first;
		TThostFtdcOrderStatusType AskOrderStatus;
		TThostFtdcOrderStatusType BidOrderStatus;
		double AskPrice=0;
		double BidPrice=0;
		
		//�볡
		if ((MarketDataField[InstrumentID].BidPrice1 > 0 )&&( MarketDataField[InstrumentID].AskPrice1 > 0)&&
			(MarketDataField[InstrumentID].BidPrice1 <10000000) && (MarketDataField[InstrumentID].AskPrice1 <10000000))
		{
			//��ί��
			if (AskORDER_REF_present[InstrumentID] != "")
			{
				if (OrderMap.count(AskORDER_REF_present[InstrumentID])>0)
				{
					AskOrderStatus = OrderMap[AskORDER_REF_present[InstrumentID]].OrderStatus;
					AskPrice = OrderMap[AskORDER_REF_present[InstrumentID]].LimitPrice;
				}
				else
				{    ///ί�б�CTP�˻�
					AskOrderStatus = 'e';
				}
			}
			else///û�п�ʼ����
				AskOrderStatus = THOST_FTDC_OST_AllTraded;

		
			//��ί��
			if (BidORDER_REF_present[InstrumentID] != "")
			{		

				if (OrderMap.count(BidORDER_REF_present[InstrumentID])>0)
				{
					BidOrderStatus = OrderMap[BidORDER_REF_present[InstrumentID]].OrderStatus;
					BidPrice = OrderMap[BidORDER_REF_present[InstrumentID]].LimitPrice;
				}
				else
				{    ///ί�б�CTP�˻�
					BidOrderStatus = 'e';
				}
			}
			else///û�п�ʼ����
				BidOrderStatus = THOST_FTDC_OST_AllTraded;			
			
			///˫�߶��ɽ����±���
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_AllTraded))
			{
				
				//#define THOST_FTDC_D_Buy 
				//#define THOST_FTDC_D_Sell		
				stringstream bidstr;				
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, MarketDataField[InstrumentID].BidPrice1 - spreed/2 , InstrumentID,volume);
				bidstr << iNextOrderRef;
				bidstr >> BidORDER_REF_present[InstrumentID];		
				
				stringstream askstr;
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID, volume);;
				askstr << iNextOrderRef;
				askstr >> AskORDER_REF_present[InstrumentID];
			}			
			///���뱨���ɽ�����������û�гɽ�
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
			{
				//#define THOST_FTDC_D_Buy 
				//#define THOST_FTDC_D_Sell			
				
				//�Ƿ��Ѿ��Ǹĵ�
				
			if (Ask_refill[InstrumentID].compare(AskORDER_REF_present[InstrumentID])!= 0)				
				{		
					//
			    //pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID);	
				//����
			    pTraderUserSpi->ReqOrderAction(&OrderMap[AskORDER_REF_present[InstrumentID]]);
				
				}
			}
			///���뱨���ɽ������������ѳ�
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_Canceled))
			{
				
				//#define THOST_FTDC_D_Buy 
				//#define THOST_FTDC_D_Sell
				//�Ƿ��Ѿ��Ǹĵ�
				if (Ask_refill[InstrumentID].compare(AskORDER_REF_present[InstrumentID]) != 0)
				{
					stringstream ss;				
					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, BidPrice + spreed, InstrumentID, volume);
					ss << iNextOrderRef;
					ss >> AskORDER_REF_present[InstrumentID];
					Ask_refill[InstrumentID] = AskORDER_REF_present[InstrumentID];
				}
			}
		
			///���������ɽ������뱨��û�гɽ�
			if ((AskOrderStatus == THOST_FTDC_OST_AllTraded) && (BidOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
			{
				//#define THOST_FTDC_D_Buy
				//#define THOST_FTDC_D_Sell 
				//�Ƿ��Ѿ��Ǹĵ�				
				if (Bid_refill[InstrumentID].compare(BidORDER_REF_present[InstrumentID])!=0)
				{
					//����
				   pTraderUserSpi->ReqOrderAction(&OrderMap[BidORDER_REF_present[InstrumentID]]);
				
			     }
			}
			///���������ɽ������뱨���ѳ�
			if ((AskOrderStatus == THOST_FTDC_OST_AllTraded) && (BidOrderStatus == THOST_FTDC_OST_Canceled))
			{
				//#define THOST_FTDC_D_Buy
				//#define THOST_FTDC_D_Sell 

				//�Ƿ��Ѿ��Ǹĵ�			

				if (Bid_refill[InstrumentID].compare(BidORDER_REF_present[InstrumentID]) != 0)
				{
					stringstream ss;				
					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, AskPrice - spreed, InstrumentID, volume);
					ss << iNextOrderRef;
					ss >> BidORDER_REF_present[InstrumentID];
					Bid_refill[InstrumentID] = BidORDER_REF_present[InstrumentID];
				}

			}

		}
	}
}

///�����߳�
void QuoteThread(CThostFtdcForQuoteRspField ForQuoteRsp) //  SubscribeForQuoteRsp
{
	////Quote������			
	string QuoteRef=pTraderUserSpi->ReqQuoteInsert(ForQuoteRsp.ForQuoteSysID,
		InstrumentMap[ForQuoteRsp.InstrumentID], ForQuoteRsp.InstrumentID, volume, spreed);

	Sleep(15000);

	pTraderUserSpi->ReqQuoteAction(&(QuoteMap[QuoteRef]));
	//����quota��    
}
//��Ӧѯ�����߳�
void RespondInquirie() //  SubscribeForQuoteRsp
{
	while (true)
	{
		if (ForQuoteRspField.size()> 0)
		{
			g_lockqueue.lock();
			CThostFtdcForQuoteRspField ForQuoteRsp = ForQuoteRspField.front();//ѯ��
			ForQuoteRspField.pop();
		    g_lockqueue.unlock();
			//string InstrumentID			
			std::thread quo(QuoteThread,ForQuoteRsp);
			quo.detach();
			///pTraderUserSpi->ReqQuoteInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID, volume);;

		}
	}
}
///��Ϣ�������߳�
void mProcess()
{
	while (true)
	{
		if (!MsgQueue.empty())
		{
			Msg msg;
			g_lockqueue.lock();
			
			msg = MsgQueue.front();

			//cerr <<      "----------------------------MSG_BEGIN-------------------------------------" << endl;
			//LOG(INFO) << "----------------------------MSG_BEGIN-------------------------------------" << endl;
			
			//cerr <<      "<��Ϣ>||����(0.ί��1.�ɽ�2.ί��¼��3.����):" << msg.Msg_Type << "|��Ϣ��:" << MsgQueue.size() << endl;
			//LOG(INFO) << "<��Ϣ>||����(0.ί��1.�ɽ�2.ί��¼��3.����):" << msg.Msg_Type << "|��Ϣ��:" << MsgQueue.size() << endl;		

		/*	cerr << "<��Ϣ>|| ��Ϣ��:" << MsgQueue.size() << endl;
			LOG(INFO) << "<��Ϣ>||��Ϣ��:" << MsgQueue.size() << endl;	*/		
			MsgQueue.pop();			
			g_lockqueue.unlock();			
			//pTraderUserSpi->IsErrorRspInfo(&msg.RspInfo);
			//��Ϣ���д���//
			switch (msg.Msg_Type)
			{
					// ί�лر�
				case RtnOrder:
				{
					//����ί�лر���״̬����**********************************************************************				
					TThostFtdcOrderStatusType OrderStatus = 'n';//ԭί��״̬����ί�г�ʼֵΪn				
					
					int Volume = -1;
					//����ί���б����Ƿ���ڴ�ί�У�ȷ����ί���Ƿ�Ϊ��ί��
				
					if (OrderMap.count(msg.RtnOrder.OrderRef) > 0)
					{
						//LOG(INFO) << "OrderStatus:" << OrderStatus << "|Volume" << Volume << endl;
						
						
						OrderStatus = OrderMap[msg.RtnOrder.OrderRef].OrderStatus;
                        ////�˴γɽ���	
						Volume = OrderMap[msg.RtnOrder.OrderRef].VolumeTotal - msg.RtnOrder.VolumeTotal;
								
					}					
					LOG(INFO) << "OrderStatusChange:" << OrderStatus << "--->" << msg.RtnOrder.OrderStatus << "|Volume:" << Volume << endl;
					
					//ί��״̬�ı� || �ɽ�״̬�ı�
					if (OrderStatus!= msg.RtnOrder.OrderStatus || Volume>0)
					{					
						///ί��¼�� ������Ӧ�ĳֲ�		
						if (OrderStatus == 'n')
						{
							LOG(INFO) << "-------------------------------OrderStatus == 'n'-----------------------------" << endl;
							LOG(INFO) << "OrderRef:" << msg.RtnOrder.OrderRef << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" << endl;
							//TThostFtdcOffsetFlagType
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.VolumeTotalOriginal);
							LOG(INFO) << "--------------------------------------------------------------------------------" << endl;
				      	 }						
						
						//ί��ȫ������ �ָ�����ĳֲ�					
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_Canceled)
						{
							LOG(INFO) << "-------------------------------THOST_FTDC_OST_Canceled---------------------------" << endl;
							LOG(INFO) << "OrderRef:" << msg.RtnOrder.OrderRef << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" << endl;
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -msg.RtnOrder.VolumeTotal);
							LOG(INFO) << "-----------------------------------------------------------------------------------" << endl;
						}

						// ί��ȫ���ɽ� �ָ�����ĳֲ� ����ֲ�
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_AllTraded)
						{
							LOG(INFO) << "-------------------------------THOST_FTDC_OST_AllTraded-----------------------------" << endl;
							LOG(INFO) << "OrderRef:" << msg.RtnOrder.OrderRef << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" << endl;							
							
							PositionChange(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], Volume);
							if (msg.RtnOrder.CombOffsetFlag[0]!= THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -Volume);
							LOG(INFO) << "-------------------------------------------------------------------------------------" << endl;
						}
					
						//���ֳɽ�
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_PartTradedQueueing)
						{
							LOG(INFO) << "-------------------------------THOST_FTDC_OST_PartTradedQueueing----------------------" << endl;
							LOG(INFO) << "OrderRef:" << msg.RtnOrder.OrderRef << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|"
								<< "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|"
								<< "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
								<< "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|" << endl;
							PositionChange(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], Volume);
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
								PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -Volume);
							LOG(INFO) << "-------------------------------------------------------------------------------------" << endl;
						}
						//���ֳ���
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_PartTradedNotQueueing)
						{
							LOG(INFO) << "------------------------------THOST_FTDC_OST_PartTradedNotQueueing-------------------" << endl;
							LOG(INFO) << "OrderRef:" << msg.RtnOrder.OrderRef << "|"
								<< "OrderStatus:" << msg.RtnOrder.OrderStatus << "|" 
							          << "VolumeTotalOriginal:" << msg.RtnOrder.VolumeTotalOriginal << "|" 
									  << "VolumeTotal:" << msg.RtnOrder.VolumeTotal << "|"
									  << "VolumeTraded:" << msg.RtnOrder.VolumeTraded << "|"<< endl;						
							
							if (msg.RtnOrder.CombOffsetFlag[0]!=THOST_FTDC_OF_Open)
							///PositionChange(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], msg.RtnOrder.VolumeTraded);
							PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, -msg.RtnOrder.VolumeTotal);
							
							LOG(INFO) << "---------------------------------------------------------------------------------------" << endl;
						}
					    /*	cerr <<     "<ί��>||���:" << msg.RtnOrder.OrderSysID << "|<" << msg.RtnOrder.InstrumentID
							<< ">����(0.��1.��):" << msg.RtnOrder.Direction << "����:" << msg.RtnOrder.VolumeTotalOriginal
							<< "|��ƽ(0.��1.ƽ):" << msg.RtnOrder.CombOffsetFlag[0]
							<< "|�۸�:" << msg.RtnOrder.LimitPrice
							<< "|״̬:" << msg.RtnOrder.OrderStatus<< endl;						
						
						LOG(INFO) << "<ί��>||���:" << msg.RtnOrder.OrderSysID << "|<" << msg.RtnOrder.InstrumentID
							<< ">����(0.��1.��):" << msg.RtnOrder.Direction << "|����:" << msg.RtnOrder.VolumeTotalOriginal
							<< "|��ƽ(0.��1.ƽ):" << msg.RtnOrder.CombOffsetFlag[0]
							<< "|�۸�:" << msg.RtnOrder.LimitPrice
							<< "|״̬:" << msg.RtnOrder.OrderStatus << endl;	*/					
						LogpInvestorPosition();					
						//����ί���б��е�״̬
						OrderMap[msg.RtnOrder.OrderRef] = msg.RtnOrder;
				/*		cerr << "----------------------------MSG_END-------------------------------------" << endl;							
				   LOG(INFO) << "----------------------------MSG_END-------------------------------------" << endl;*/
						//ִ�в���
						//QuotaStrategy();						
					}				
					break;
				};		
				// �ɽ��ر�
				case RtnQuote:
				{
					
					QuoteMap[msg.RtnQuote.QuoteRef] = msg.RtnQuote;
					
					//g_lockqueue.lock();

					//TradeList.push_back(msg.RtnTrade);

					//PositionChange(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);

					//PositionFrozen(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, -msg.RtnTrade.Volume);

					//LogpInvestorPosition();

					//g_lockqueue.unlock();

					/*	cerr << "<�ɽ�>||���:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
					<< ">����(0.��1.��):" << msg.RtnTrade.Direction << "����:" << msg.RtnTrade.Volume
					<< "|��ƽ(0.��1.ƽ):" << msg.RtnTrade.OffsetFlag
					<< "|�۸�:" << msg.RtnTrade.Price << endl;

					LOG(INFO) << "<�ɽ�>||���:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
					<< ">����(0.��1.��):" << msg.RtnTrade.Direction << "����:" << msg.RtnTrade.Volume
					<< "|��ƽ(0.��1.ƽ):" << msg.RtnTrade.OffsetFlag
					<< "|�۸�:" << msg.RtnTrade.Price << endl;

					cerr << "----------------------------MSG_END-------------------------------------" << endl;
					LOG(INFO) << "----------------------------MSG_END-------------------------------------" << endl;*/

					break;
				};
				
				// �ɽ��ر�
				case RtnTrade:
				{			
					//g_lockqueue.lock();

					//TradeList.push_back(msg.RtnTrade);
				
					//PositionChange(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);
				
					//PositionFrozen(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, -msg.RtnTrade.Volume);
				
					//LogpInvestorPosition();
				
					//g_lockqueue.unlock();

				/*	cerr << "<�ɽ�>||���:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
						<< ">����(0.��1.��):" << msg.RtnTrade.Direction << "����:" << msg.RtnTrade.Volume
						<< "|��ƽ(0.��1.ƽ):" << msg.RtnTrade.OffsetFlag
						<< "|�۸�:" << msg.RtnTrade.Price << endl;

					LOG(INFO) << "<�ɽ�>||���:" << msg.RtnTrade.OrderSysID << "|<" << msg.RtnTrade.InstrumentID
						<< ">����(0.��1.��):" << msg.RtnTrade.Direction << "����:" << msg.RtnTrade.Volume
						<< "|��ƽ(0.��1.ƽ):" << msg.RtnTrade.OffsetFlag
						<< "|�۸�:" << msg.RtnTrade.Price << endl;
					
					cerr << "----------------------------MSG_END-------------------------------------" << endl;
					LOG(INFO) << "----------------------------MSG_END-------------------------------------" << endl;*/

					break;
				};
			
				//����¼��
				case InputOrder:
				{
					break;
				};
				// ��������¼��
				case InputOrderAction:
				{
					break;
				}
			}

		
		}
		 
	}
}
void main(void)
{	
	
	//LOG
	google::InitGoogleLogging("Quote");  //����Ϊ�Լ��Ŀ�ִ���ļ��� 		

	google::SetLogDestination(google::GLOG_INFO, "./Log/LOG");

	FLAGS_max_log_size = 50;  //�����־��СΪ 50MB

	SysInit();//��ʼ��ȫ�ֱ���

	TThostFtdcCommandTypeType MdFront;

	TThostFtdcCommandTypeType TraderFront;

	//TThostFtdcBrokerIDType	BROKER_ID = "7080";			
	
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(CThostFtdcReqUserLoginField));
	memcpy(req.BrokerID, BROKERID.c_str(), sizeof(TThostFtdcBrokerIDType));
	memcpy(req.UserID, INVESTOR_ID.c_str(), sizeof(TThostFtdcUserIDType));
	memcpy(req.Password, PASSWORD.c_str(), sizeof(TThostFtdcPasswordType));	
	//�ͻ��˵�¼��֤��Ϣ
	CThostFtdcReqAuthenticateField  Authenticate;
	memset(&Authenticate, 0, sizeof(CThostFtdcReqUserLoginField));
	memcpy(Authenticate.BrokerID, BROKERID.c_str(), sizeof(TThostFtdcBrokerIDType));
	memcpy(Authenticate.UserID, INVESTOR_ID.c_str(), sizeof(TThostFtdcUserIDType));
	memcpy(Authenticate.UserProductInfo, strUserProductInfo.c_str(), sizeof(TThostFtdcProductInfoType));
	memcpy(Authenticate.AuthCode, strAuthCode.c_str(), sizeof(TThostFtdcAuthCodeType));	
	
	memset(&MdFront, 0, sizeof(TThostFtdcCommandTypeType));
	memcpy(MdFront, MDFRONT_ADDR.c_str(), sizeof(TThostFtdcCommandTypeType));

	memset(&TraderFront, 0, sizeof(TThostFtdcCommandTypeType));
	memcpy(TraderFront, TRADERFRONT_ADDR.c_str(), sizeof(TThostFtdcCommandTypeType));

	//��ʼ������UserApi
	CThostFtdcTraderApi *pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi

	pTraderUserSpi = new CTraderSpi(pTraderUserApi, req,Authenticate);

	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// ע���¼���

	pTraderUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ����
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// ע��˽����
	pTraderUserApi->RegisterFront(TraderFront);							// connect
	pTraderUserApi->Init();

	// ��ʼ������UserApi
	CThostFtdcMdApi* pMDUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi

	pMDUserSpi = new CMdSpi(pMDUserApi, req);

	pMDUserApi->RegisterSpi(pMDUserSpi);						// ע���¼���

	pMDUserApi->RegisterFront(MdFront);					// connect

	pMDUserApi->Init();

	//�ȴ�ϵͳ��ʼ�����
	while (!InitFinished)
	  Sleep(1000);
	
	//ִ�в���
	//QuotaStrategy();
		
	//�ػ��߳�	
	std::thread daemon(mProcess);
	//daemon.detach();

	
	//��Ӧѯ���߳�
	//std::thread th(RespondInquirie);
	daemon.join();




}




