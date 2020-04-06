#include "RTC.h"

// Use the 32.768 kHz low-speed external clock as RTC clock source

// Helper macro to convert a value from 2 digit decimal format to BCD format
// __VALUE__ Byte to be converted
#define __RTC_CONVERT_BIN2BCD(__VALUE__) (uint8_t)((((__VALUE__) / 10) << 4) | ((__VALUE__) % 10))
#define __RTC_CONVERT_BCD2BIN(__VALUE__) (uint8_t)(((uint8_t)((__VALUE__) & (uint8_t)0xF0) >> (uint8_t)0x4) * 10 + ((__VALUE__) & (uint8_t)0x0F))

#define RTC_WEEKDAY_MONDAY              ((uint32_t)0x01) /*!< Monday    */
#define RTC_WEEKDAY_TUESDAY             ((uint32_t)0x02) /*!< Tuesday   */
#define RTC_WEEKDAY_WEDNESDAY           ((uint32_t)0x03) /*!< Wednesday */
#define RTC_WEEKDAY_THURSDAY            ((uint32_t)0x04) /*!< Thrusday  */
#define RTC_WEEKDAY_FRIDAY              ((uint32_t)0x05) /*!< Friday    */
#define RTC_WEEKDAY_SATURDAY            ((uint32_t)0x06) /*!< Saturday  */
#define RTC_WEEKDAY_SUNDAY              ((uint32_t)0x07) /*!< Sunday    */

#define RTC_MONTH_JANUARY               ((uint8_t)0x01)  /*!< January   */
#define RTC_MONTH_FEBRUARY              ((uint8_t)0x02)  /*!< February  */
#define RTC_MONTH_MARCH                 ((uint8_t)0x03)  /*!< March     */
#define RTC_MONTH_APRIL                 ((uint8_t)0x04)  /*!< April     */
#define RTC_MONTH_MAY                   ((uint8_t)0x05)  /*!< May       */
#define RTC_MONTH_JUNE                  ((uint8_t)0x06)  /*!< June      */
#define RTC_MONTH_JULY                  ((uint8_t)0x07)  /*!< July      */
#define RTC_MONTH_AUGUST                ((uint8_t)0x08)  /*!< August    */
#define RTC_MONTH_SEPTEMBER             ((uint8_t)0x09)  /*!< September */
#define RTC_MONTH_OCTOBER               ((uint8_t)0x10)  /*!< October   */
#define RTC_MONTH_NOVEMBER              ((uint8_t)0x11)  /*!< November  */
#define RTC_MONTH_DECEMBER              ((uint8_t)0x12)  /*!< December  */

void RTC_BAK_SetRegister(uint32_t BackupRegister, uint32_t Data) {
	register uint32_t tmp = 0;

	tmp = (uint32_t)(&(RTC->BKP0R));
	tmp += (BackupRegister * 4);

	/* Write the specified register */
	*(__IO uint32_t *)tmp = (uint32_t)Data;
}

void RTC_Init(void) {
	
	/* Enables the PWR Clock and Enables access to the backup domain #######*/
	/* To change the source clock of the RTC feature (LSE, LSI), you have to:
	 - Enable the power clock
	 - Enable write access to configure the RTC clock source (to be done once after reset).
	 - Reset the Back up Domain
	 - Configure the needed RTC clock source */
	RTC_Clock_Init();
	
	// Disable RTC registers write protection
	RTC_Disable_Write_Protection();
	
	// Enter in initialization mode
	RTC->ISR |= RTC_ISR_INIT;
	while( (RTC->ISR & RTC_ISR_INITF) == 0); // Wait until INITF is set after entering initialization mode
	
	// Configure the Date 
	/* Note: __LL_RTC_CONVERT_BIN2BCD helper macro can be used if user wants to */
	/*       provide directly the decimal value:                                */
	RTC_Set_Calendar_Date(RTC_WEEKDAY_TUESDAY, 0x17, RTC_MONTH_DECEMBER, 0x29); /* STUB (Wednesday January 1, 2020): Fill in current date */
	
	// Configure the Time 
	RTC_Set_Time(0x00, 0x15, 0x59, 0x51); /* STUB (7:00:00 PM): Fill in current time */
  
	// Exit of initialization mode 
	RTC->ISR &= ~RTC_ISR_INIT;
	while((RTC->ISR & RTC_ISR_RSF) == 0); /* Wait for synchro */
                                          /* Note: Needed only if Shadow registers is enabled */
                                          /* LL_RTC_IsShadowRegBypassEnabled function can be used */
	
	// Enable RTC registers write protection 
	RTC_Enable_Write_Protection();

	// Writes a data in a RTC Backup data Register1 
	// to indicate date/time updated 
	RTC_BAK_SetRegister(1, 0x32F2);
}

#define POSITION_VAL(VAL)     (__CLZ(__RBIT(VAL)))
#define RTC_POSITION_TR_HT    (uint32_t)POSITION_VAL(RTC_TR_HT)
#define RTC_POSITION_TR_HU    (uint32_t)POSITION_VAL(RTC_TR_HU)
#define RTC_POSITION_TR_MT    (uint32_t)POSITION_VAL(RTC_TR_MNT)
#define RTC_POSITION_TR_MU    (uint32_t)POSITION_VAL(RTC_TR_MNU)
#define RTC_POSITION_TR_ST    (uint32_t)POSITION_VAL(RTC_TR_ST)
#define RTC_POSITION_TR_SU    (uint32_t)POSITION_VAL(RTC_TR_SU)
#define RTC_POSITION_DR_YT    (uint32_t)POSITION_VAL(RTC_DR_YT)
#define RTC_POSITION_DR_YU    (uint32_t)POSITION_VAL(RTC_DR_YU)
#define RTC_POSITION_DR_MT    (uint32_t)POSITION_VAL(RTC_DR_MT)
#define RTC_POSITION_DR_MU    (uint32_t)POSITION_VAL(RTC_DR_MU)
#define RTC_POSITION_DR_DT    (uint32_t)POSITION_VAL(RTC_DR_DT)
#define RTC_POSITION_DR_DU    (uint32_t)POSITION_VAL(RTC_DR_DU)
#define RTC_POSITION_DR_WDU   (uint32_t)POSITION_VAL(RTC_DR_WDU)

void RTC_Set_Calendar_Date(uint32_t WeekDay, uint32_t Day, uint32_t Month, uint32_t Year) {
	RTC->DR &= ~((uint32_t)0x00FFFF3FU);
	RTC->DR |= Year<<16 | WeekDay<<13 | Month << 8 | Day;
}

void RTC_Set_Time(uint32_t Format12_24, uint32_t Hour, uint32_t Minute, uint32_t Second) {
	RTC->TR |= Format12_24<<22 | Hour<<16 | Minute<<8 | Second;
}

void RTC_Clock_Init(void) {
	
	// Enable write access to Backup domain
	if ( (RCC->APB1ENR1 & RCC_APB1ENR1_PWREN) == 0 ) {
		RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;	// Power interface clock enable
		(void) RCC->APB1ENR1;  // Delay after an RCC peripheral clock enabling
	}
	
	// Select LSE as RTC clock source 
	if ( (PWR->CR1 & PWR_CR1_DBP) == 0) {
		PWR->CR1  |= PWR_CR1_DBP;				  			// Enable write access to Backup domain
		while((PWR->CR1 & PWR_CR1_DBP) == 0);  	// Wait for Backup domain Write protection disable
	}
	
	// Reset LSEON and LSEBYP bits before configuring the LSE
	RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);

	// RTC Clock selection can be changed only if the Backup Domain is reset
	RCC->BDCR |=  RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;
	
	// Note from STM32L4 Reference Manual: 	
	// RTC/LCD Clock:  (1) LSE is in the Backup domain. (2) HSE and LSI are not.	
	while((RCC->BDCR & RCC_BDCR_LSERDY) == 0){  // Wait until LSE clock ready
		RCC->BDCR |= RCC_BDCR_LSEON;
	}
	
	// Select LSE as RTC clock source
	// BDCR = Backup Domain Control Register 
	RCC->BDCR	&= ~RCC_BDCR_RTCSEL;	  // RTCSEL[1:0]: 00 = No Clock, 01 = LSE, 10 = LSI, 11 = HSE
	RCC->BDCR	|= RCC_BDCR_RTCSEL_0;   // Select LSE as RTC clock	
	
	RCC->BDCR |= RCC_BDCR_RTCEN;      // Enable RTC
	
	RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN;	// Power interface clock disable
}

void RTC_Disable_Write_Protection(void) {
	RTC->WPR = ((uint8_t)0xCAU);
	RTC->WPR = ((uint8_t)0x53U);
}
	
void RTC_Enable_Write_Protection(void) {
	RTC->WPR = ((uint8_t)0x00U);
}

uint8_t RTC_TIME_GetHour(void) {
	uint32_t time = RTC->TR;
	time &= ~((uint32_t)0xFFC0FFFFU);
	return __RTC_CONVERT_BCD2BIN(time>>16);
}

uint8_t RTC_TIME_GetMinute(void) {
	uint32_t time = RTC->TR;
	time &= ~((uint32_t)0xFFFF80FFU);
	return __RTC_CONVERT_BCD2BIN(time>>8);
}

uint8_t RTC_TIME_GetSecond(void) {
	uint32_t time = RTC->TR;
	time &= ~((uint32_t)0xFFFFFF80U);
	return __RTC_CONVERT_BCD2BIN(time);
}

uint32_t RTC_DATE_GetMonth(void) {
	uint32_t date = RTC->DR;
	date &= ~((uint32_t)0xFFFFE0FFU);
	return date>>8;
}

uint32_t RTC_DATE_GetDay(void) {
	uint32_t date = RTC->DR;
	date &= ~((uint32_t)0xFFFFFFC0U);
	return date;
}

uint32_t RTC_DATE_GetYear(void) {
	uint32_t date = RTC->DR;
	date &= ~((uint32_t)0xFF00FFFFU);
	return date>>16;
}

uint32_t RTC_DATE_GetWeekDay(void) {
	uint32_t date = RTC->DR;
	date &= ~((uint32_t)0xFFFF1FFFU);
	return date>>13;
}

void Get_RTC_Calendar(char * strTime, char * strDate) {
	/* Note: need to convert in decimal value in using __LL_RTC_CONVERT_BCD2BIN helper macro */
	/* Display time Format : hh:mm:ss */
	sprintf((char*)strTime,"%.2d:%.2d:%.2d", 
		RTC_TIME_GetHour(), 
		RTC_TIME_GetMinute(), 
		RTC_TIME_GetSecond());
	/* Display date Format : mm-dd-yy */
	sprintf((char*)strDate,"%.2d-%.2d-%.2d", 
		__RTC_CONVERT_BCD2BIN(RTC_DATE_GetMonth()), 
		__RTC_CONVERT_BCD2BIN(RTC_DATE_GetDay()), 
		2000 + __RTC_CONVERT_BCD2BIN(RTC_DATE_GetYear()));
}
