/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/

#include "bsp.h"
#include "lcd_driver.h"
#include "user_com.h"
#include "user_app.h"
#include "menu.h"
#include "user_meter.h"
#include "user_menu.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

uint32_t baseTime = 0;
uint32_t testTime = 0;

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

void SystemClock_Config(void);

/* Private user code ---------------------------------------------------------*/

/**
 * @brief 将HSV颜色转换为RGB颜色
 *
 * @param h 色调 (0-360)
 * @param s 饱和度 (0-255)
 * @param v 亮度 (0-255)
 * @param r 输出红色分量 (0-255)
 * @param g 输出绿色分量 (0-255)
 * @param b 输出蓝色分量 (0-255)
 */
void HSVtoRGB(uint16_t h, uint8_t s, uint8_t v, uint8_t* r, uint8_t* g, uint8_t* b)
{
    uint8_t region, remainder, p, q, t;

    if (s == 0)  // 无饱和度，灰色
    {
        *r = *g = *b = v;
        return;
    }
   
    region    = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
        case 0:
            *r = v;
            *g = t;
            *b = p;
            break;
        case 1:
            *r = q;
            *g = v;
            *b = p;
            break;
        case 2:
            *r = p;
            *g = v;
            *b = t;
            break;
        case 3:
            *r = p;
            *g = q;
            *b = v;
            break;
        case 4:
            *r = t;
            *g = p;
            *b = v;
            break;
        default:
            *r = v;
            *g = p;
            *b = q;
            break;
    }
}

/**
 * @brief  The application entry point.
 * @retval int
 */

int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

   /* Configure the system clock */
    SystemClock_Config();
 
#if (POWERON_DELAY_ENABLE)
    HAL_Delay(POWERON_DELAY_TIME);
#endif

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_SPI1_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
		
    LoadParam();
    bsp_Init();

    LCD_init();
    user_app_init();
		
		LED1_ON(false);
		LED2_ON(false); 
		/* Infinite loop */
    while (1)
    {		
//			  baseTime = bsp_GetRunTime() ;
			
        user_app_task();
        if (g_uart2_idle_pending)
        {
            g_uart2_idle_pending = 0;
            user_meter_uart_task();
        }
        if (g_tVar.NeedMeasureFunc)  // 上位机切换测试通道
        {
            g_tVar.NeedMeasureFunc = FALSE;
            user_meter_measure_switch(User_Meter.MeasureMode);
        }
        else if (g_tVar.MenuJump)  // 上位机菜单跳转指令
        {
            if (!Menu_IsRun())
            {
                user_menu_init();
            }
            uint8_t items[] = {g_tVar.MenuJump & 0xFF};
            Menu_ShortcutEnter((g_tVar.MenuJump >> 16 & 0xFF), (g_tVar.MenuJump >> 8 & 0xFF), items);
            g_tVar.MenuJump = 0;
        }
        else if (g_tVar.NeedSaveParam)
        {
            g_tVar.NeedSaveParam = 0;
						SaveParam();
        }				
//				testTime = bsp_GetRunTime() - baseTime;

    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM            = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN            = 8;
    RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ            = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR            = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
