/**
**********************************************************************************************************************
* @file    menu.c
* @brief   ???????????
* @author  const_zpc  any question please send mail to const_zpc@163.com
* @version V3.1.0
* @date    2022-09-04
*
* @details  ??????:
*           + ???????
*           + ???????
*           + ??????
*           + ????????
*
**********************************************************************************************************************
* ????:https://gitee.com/const-zpc/menu.git ?????????????? Issue
*
* ????:
*    1????????? Menu_Init, ???????
*    2??????? Menu_Task, ?????????????????
*    3??????????????
*
**********************************************************************************************************************
*/

/* Includes ----------------------------------------------------------------------------------------------------------*/
#include "menu.h"

#ifdef _MENU_USE_MALLOC_
#include <malloc.h>
#endif

#ifdef _MENU_USE_SHORTCUT_
#include <stdarg.h>
#endif

/* Private typedef ---------------------------------------------------------------------------------------------------*/
typedef struct MenuCtrl
{
    struct MenuCtrl  *pParentMenuCtrl; /*!< ??????? */
    char             *pszDesc;         /*!< ???????????? */
    char             *pszEnDesc;       /*!< ???????????? */
    ShowMenuCallFun_f pfnShowMenuFun;  /*!< ?????????? */
    MenuList_t       *pMenuList;       /*!< ?????? */
    MenuCallFun_f     pfnLoadCallFun;  /*!< ???????? */
    MenuCallFun_f     pfnRunCallFun;   /*!< ???????????? */
    menusize_t        itemsNum;        /*!< ????????? */
    menusize_t        showBaseItem;    /*!< ??????????? */
    menusize_t        selectItem;      /*!< ????????? */
    bool              isSelected;      /*!< ??????????? */
} MenuCtrl_t;

typedef struct
{
    MenuCtrl_t *pMenuCtrl; /*!< ???????? */
    bool        isEnglish; /*!< ?????? */
} MenuManage_t;

/* Private define ----------------------------------------------------------------------------------------------------*/
/* Private macro -----------------------------------------------------------------------------------------------------*/
/* Private variables -------------------------------------------------------------------------------------------------*/
static MenuManage_t sg_tMenuManage;

#ifndef _MENU_USE_MALLOC_
static MenuCtrl_t sg_arrMenuCtrl[MENU_MAX_DEPTH];
#endif

static uint8_t sg_currMenuDepth = 0;

/* Private function prototypes ---------------------------------------------------------------------------------------*/
static MenuCtrl_t *NewMenu(void);
static void        DeleteMenu(MenuCtrl_t *pMenu);
static MenuCtrl_t *MianMenu(void);

/* Private function --------------------------------------------------------------------------------------------------*/
/**
 * @brief      ??????
 *
 * @return     MenuCtrl_t*
 */
static MenuCtrl_t *NewMenu(void)
{
    MenuCtrl_t *pMenuCtrl = NULL;

    if (sg_currMenuDepth < MENU_MAX_DEPTH)
    {
#ifdef _MENU_USE_MALLOC_
        pMenuCtrl = (MenuCtrl_t *)malloc(sizeof(MenuCtrl_t));
#else
        pMenuCtrl = &sg_arrMenuCtrl[sg_currMenuDepth];
#endif
        sg_currMenuDepth++;
    }

    return pMenuCtrl;
}

/**
 * @brief      ??????
 *
 * @param      pMenu
 */
static void DeleteMenu(MenuCtrl_t *pMenu)
{
#ifdef _MENU_USE_MALLOC_
    free(pMenu);
#endif
    if (sg_currMenuDepth > 0)
    {
        sg_currMenuDepth--;
    }
}

/**
 * @brief      ???????????
 *
 * @return     MenuCtrl_t*
 */
static MenuCtrl_t *MianMenu(void)
{
    MenuCtrl_t *pMenuCtrl = sg_tMenuManage.pMenuCtrl;

    while (pMenuCtrl != NULL && pMenuCtrl->pParentMenuCtrl != NULL)
    {
        pMenuCtrl = pMenuCtrl->pParentMenuCtrl;
    }

    return pMenuCtrl;
}

/**
 * @brief      ?????
 *
 * @param[in]  pMainMenu        ???????
 * @return     0,??; -1,??
 */
int Menu_Init(MainMenuCfg_t *pMainMenu)
{
    MenuCtrl_t *pNewMenuCtrl = NULL;

    if (sg_tMenuManage.pMenuCtrl != NULL)
    {
        return -1;
    }

#if MENU_MAX_DEPTH != 0
    sg_currMenuDepth = 0;
#endif

    if ((pNewMenuCtrl = NewMenu()) == NULL)
    {
        return -1;
    }

    sg_tMenuManage.isEnglish = false;

    pNewMenuCtrl->pszDesc         = (char *)pMainMenu->pszDesc;
    pNewMenuCtrl->pszEnDesc       = (char *)pMainMenu->pszEnDesc;
    pNewMenuCtrl->pParentMenuCtrl = NULL;
    pNewMenuCtrl->pfnLoadCallFun  = pMainMenu->pfnLoadCallFun;
    pNewMenuCtrl->pfnShowMenuFun  = NULL;
    pNewMenuCtrl->pfnRunCallFun   = pMainMenu->pfnRunCallFun;

    pNewMenuCtrl->pMenuList    = NULL;
    pNewMenuCtrl->itemsNum     = 0;
    pNewMenuCtrl->selectItem   = 0;
    pNewMenuCtrl->showBaseItem = 0;

    sg_tMenuManage.pMenuCtrl = pNewMenuCtrl;

    if (sg_tMenuManage.pMenuCtrl->pfnLoadCallFun != NULL)
    {
        sg_tMenuManage.pMenuCtrl->pfnLoadCallFun();
    }

    return 0;
}

/**
 * @brief  ??????
 *
 * @return 0,??; -1,??
 */
int Menu_DeInit(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    while (Menu_Exit(1) == 0)
    {
    }

    DeleteMenu(sg_tMenuManage.pMenuCtrl);
    sg_tMenuManage.pMenuCtrl = NULL;
    sg_tMenuManage.isEnglish = false;

    return 0;
}

/**
 * @brief      ???????????
 *
 * @param      pMenuList       ??????
 * @param      menuNum         ????????
 * @param      pfnShowMenuFun  ??????????????, ?NULL???????????
 * @return     int
 */
int Menu_Bind(MenuList_t *pMenuList, menusize_t menuNum, ShowMenuCallFun_f pfnShowMenuFun)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->pMenuList != NULL)
    {
        return 0;
    }

    sg_tMenuManage.pMenuCtrl->pMenuList = pMenuList;
    sg_tMenuManage.pMenuCtrl->itemsNum  = menuNum;

    if (pfnShowMenuFun != NULL)
    {
        sg_tMenuManage.pMenuCtrl->pfnShowMenuFun = pfnShowMenuFun;
    }

    return 0;
}

/**
 * @brief      ???????
 *
 * @return     false,??; true,??
 */
bool Menu_IsEnglish(void)
{
    return sg_tMenuManage.isEnglish;
}

/**
 * @brief      ??????
 *
 * @param[in]  isEnable ??????
 * @return     0,??; -1,??
 */
int Menu_EnableEnglish(bool isEnable)
{
    sg_tMenuManage.isEnglish = isEnable;
    return 0;
}

/**
 * @brief      ????, ???????
 *
 * @note       ????????????????????????
 * @return     0,??; -1,??
 */
int Menu_Reset(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    while (sg_tMenuManage.pMenuCtrl->pParentMenuCtrl != NULL)
    {
        MenuCtrl_t *pMenuCtrl = sg_tMenuManage.pMenuCtrl;

        sg_tMenuManage.pMenuCtrl = sg_tMenuManage.pMenuCtrl->pParentMenuCtrl;
        DeleteMenu(pMenuCtrl);
    }

    sg_tMenuManage.pMenuCtrl->selectItem = 0;

    return 0;
}

/**
 * @brief      ??????????
 *
 * @return     false,?????; true,????
 */
bool Menu_IsRun(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return false;
    }

    return true;
}

/**
 * @brief      ????????
 *
 * @return     0,??; -1,??
 */
int Menu_Enter(void)
{
    MenuCtrl_t *pNewMenuCtrl  = NULL;
    MenuCtrl_t *pCurrMenuCtrl = sg_tMenuManage.pMenuCtrl;

    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if ((pNewMenuCtrl = NewMenu()) == NULL)
    {
        return -1;
    }

    pNewMenuCtrl->pszDesc         = (char *)pCurrMenuCtrl->pMenuList[pCurrMenuCtrl->selectItem].pszDesc;
    pNewMenuCtrl->pszEnDesc       = (char *)pCurrMenuCtrl->pMenuList[pCurrMenuCtrl->selectItem].pszEnDesc;
    pNewMenuCtrl->pMenuList       = NULL;
    pNewMenuCtrl->itemsNum        = 0;
    pNewMenuCtrl->pfnShowMenuFun  = pCurrMenuCtrl->pfnShowMenuFun;
    pNewMenuCtrl->pfnLoadCallFun  = pCurrMenuCtrl->pMenuList[pCurrMenuCtrl->selectItem].pfnLoadCallFun;
    pNewMenuCtrl->pfnRunCallFun   = pCurrMenuCtrl->pMenuList[pCurrMenuCtrl->selectItem].pfnRunCallFun;
    pNewMenuCtrl->selectItem      = 0;
    pNewMenuCtrl->isSelected      = true;
    pNewMenuCtrl->pParentMenuCtrl = pCurrMenuCtrl;

    sg_tMenuManage.pMenuCtrl = pNewMenuCtrl;

    if (sg_tMenuManage.pMenuCtrl->pfnLoadCallFun != NULL)
    {
        sg_tMenuManage.pMenuCtrl->pfnLoadCallFun();
    }

    return 0;
}

/**
 * @brief      ??????????????
 *
 * @param[in]  isReset ??????????
 * @return     0,??; -1,??, ????????, ????
 */
int Menu_Exit(bool isReset)
{
    MenuCtrl_t *pMenuCtrl = sg_tMenuManage.pMenuCtrl;

    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->pParentMenuCtrl == NULL)
    {
        return -1;
    }

    sg_tMenuManage.pMenuCtrl = sg_tMenuManage.pMenuCtrl->pParentMenuCtrl;
    DeleteMenu(pMenuCtrl);
    pMenuCtrl = NULL;

    if (sg_tMenuManage.pMenuCtrl->pMenuList[sg_tMenuManage.pMenuCtrl->selectItem].pfnExitCallFun != NULL)
    {
        sg_tMenuManage.pMenuCtrl->isSelected = false;
        sg_tMenuManage.pMenuCtrl->pMenuList[sg_tMenuManage.pMenuCtrl->selectItem].pfnExitCallFun();
    }

    if (sg_tMenuManage.pMenuCtrl->pfnLoadCallFun != NULL)
    {
        sg_tMenuManage.pMenuCtrl->pfnLoadCallFun();
    }

    if (isReset)
    {
        sg_tMenuManage.pMenuCtrl->selectItem = 0;
    }

    return 0;
}

/**
 * @brief      ?????????
 *
 * @param[in]  isAllowRoll ??????????????????
 * @return     0,??; -1,??
 */
int Menu_SelectPrevious(bool isAllowRoll)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->selectItem > 0)
    {
        sg_tMenuManage.pMenuCtrl->selectItem--;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pMenuCtrl->selectItem = sg_tMenuManage.pMenuCtrl->itemsNum - 1;
        }
        else
        {
            sg_tMenuManage.pMenuCtrl->selectItem = 0;
            return -1;
        }
    }

    return 0;
}

/**
 * @brief      ?????????
 *
 * @param[in]  isAllowRoll ?????????????????
 * @return     0,??; -1,??
 */
int Menu_SelectNext(bool isAllowRoll)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (sg_tMenuManage.pMenuCtrl->selectItem < (sg_tMenuManage.pMenuCtrl->itemsNum - 1))
    {
        sg_tMenuManage.pMenuCtrl->selectItem++;
    }
    else
    {
        if (isAllowRoll)
        {
            sg_tMenuManage.pMenuCtrl->selectItem = 0;
        }
        else
        {
            sg_tMenuManage.pMenuCtrl->selectItem = sg_tMenuManage.pMenuCtrl->itemsNum - 1;
            return -1;
        }
    }

    return 0;
}

#ifdef _MENU_USE_SHORTCUT_

/**
 * @brief      ???????????????????????????
 *
 * @param[in]  isAbsolute ??????????(??????)
 * @param[in]  deep       ????,?? 0
 * @param[in]  items      ?????????(?0??),????? deep ????
 * @return     0,??; -1,??
 */
int Menu_ShortcutEnter(bool isAbsolute, uint8_t deep, const menusize_t *items)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    if (isAbsolute)
    {
        if (Menu_Reset() != 0)
        {
            return -1;
        }
    }

    if (deep == 0 || items == NULL)
    {
        return -1;
    }

    for (uint8_t i = 0; i < deep; ++i)
    {
        menusize_t selectItem = items[i];

        if (selectItem >= sg_tMenuManage.pMenuCtrl->itemsNum || selectItem < 0)
        {
            return -1;
        }

        sg_tMenuManage.pMenuCtrl->selectItem = selectItem;

        if (Menu_Enter() != 0)
        {
            return -1;
        }
    }

    return 0;
}

#endif

/**
 * @brief      ?????????????????
 *
 * @note       ??????????????, ?????? showBaseItem ??????????????
 * @param[in,out]  tMenuShow   ????????
 * @param[in,out]  showNum     ??????????????, ???????????????????????
 * @return     0,??; -1,??
 */
int Menu_LimitShowListNum(MenuShow_t *ptMenuShow, menusize_t *pShowNum)
{
    if (ptMenuShow == NULL || pShowNum == NULL)
    {
        return -1;
    }

    if (*pShowNum > ptMenuShow->itemsNum)
    {
        *pShowNum = ptMenuShow->itemsNum;
    }

    if (ptMenuShow->selectItem < ptMenuShow->showBaseItem)
    {
        ptMenuShow->showBaseItem = ptMenuShow->selectItem;
    }
    else if ((ptMenuShow->selectItem - ptMenuShow->showBaseItem) >= *pShowNum)
    {
        ptMenuShow->showBaseItem = ptMenuShow->selectItem - *pShowNum + 1;
    }
    else
    {
        // ??
    }

    return 0;
}

/**
 * @brief       ???????????
 *              ???????????????,level ?2
 *
 * @param[out]  ptMenuShow ? n ???????
 * @param[in]   level      n ?, ?? 0
 * @return int
 */
int Menu_QueryParentMenu(MenuShow_t *ptMenuShow, uint8_t level)
{
    int         i;
    MenuList_t *pMenu;
    MenuCtrl_t *pMenuCtrl = NULL;

    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return -1;
    }

    pMenuCtrl = sg_tMenuManage.pMenuCtrl->pParentMenuCtrl;

    while (level && pMenuCtrl != NULL)
    {
        pMenu                    = pMenuCtrl->pMenuList;
        ptMenuShow->itemsNum     = pMenuCtrl->itemsNum;
        ptMenuShow->selectItem   = pMenuCtrl->selectItem;
        ptMenuShow->showBaseItem = pMenuCtrl->showBaseItem;

        if (sg_tMenuManage.isEnglish)
        {
            ptMenuShow->pszDesc = pMenuCtrl->pszEnDesc;

            for (i = 0; i < ptMenuShow->itemsNum && i < MENU_MAX_NUM; i++)
            {
                ptMenuShow->pszItemsDesc[i] = (char *)pMenu[i].pszEnDesc;
                ptMenuShow->pItemsExData[i] = pMenu[i].pExtendData;
            }
        }
        else
        {
            ptMenuShow->pszDesc = pMenuCtrl->pszDesc;

            for (i = 0; i < ptMenuShow->itemsNum && i < MENU_MAX_NUM; i++)
            {
                ptMenuShow->pszItemsDesc[i] = (char *)pMenu[i].pszDesc;
                ptMenuShow->pItemsExData[i] = pMenu[i].pExtendData;
            }
        }

        pMenuCtrl = pMenuCtrl->pParentMenuCtrl;
        level--;
    }

    if (level != 0 && pMenuCtrl == NULL)
    {
        return -1;
    }

    return 0;
}

int Menu_QuerySelectItem(void)
{
    if (sg_tMenuManage.pMenuCtrl == NULL)
        return -1;
    else
        return sg_tMenuManage.pMenuCtrl->selectItem;
}

/**
 * @brief  ????
 *
 * @return 0,??, ???????; -1,??, ????????
 */
void Menu_Task(void)
{
    int         i;
    MenuList_t *pMenuList;
    MenuShow_t  tMenuShow;

    if (sg_tMenuManage.pMenuCtrl == NULL)
    {
        return;
    }

    if (sg_tMenuManage.pMenuCtrl->pfnRunCallFun != NULL)
    {
        sg_tMenuManage.pMenuCtrl->pfnRunCallFun();
        if (sg_tMenuManage.pMenuCtrl == NULL)
        {
            return;
        }
    }

    if (sg_tMenuManage.pMenuCtrl->pMenuList != NULL)
    {
        pMenuList              = sg_tMenuManage.pMenuCtrl->pMenuList;
        tMenuShow.itemsNum     = sg_tMenuManage.pMenuCtrl->itemsNum;
        tMenuShow.selectItem   = sg_tMenuManage.pMenuCtrl->selectItem;
        tMenuShow.showBaseItem = sg_tMenuManage.pMenuCtrl->showBaseItem;

        if (sg_tMenuManage.isEnglish)
        {
            tMenuShow.pszDesc = sg_tMenuManage.pMenuCtrl->pszEnDesc;

            for (i = 0; i < tMenuShow.itemsNum && i < MENU_MAX_NUM; i++)
            {
                tMenuShow.pszItemsDesc[i] = (char *)pMenuList[i].pszEnDesc;
                tMenuShow.pItemsExData[i] = pMenuList[i].pExtendData;
            }
        }
        else
        {
            tMenuShow.pszDesc = sg_tMenuManage.pMenuCtrl->pszDesc;

            for (i = 0; i < tMenuShow.itemsNum && i < MENU_MAX_NUM; i++)
            {
                tMenuShow.pszItemsDesc[i] = (char *)pMenuList[i].pszDesc;
                tMenuShow.pItemsExData[i] = pMenuList[i].pExtendData;
            }
        }

        if (sg_tMenuManage.pMenuCtrl->pfnShowMenuFun != NULL)
        {
            sg_tMenuManage.pMenuCtrl->pfnShowMenuFun(&tMenuShow);
        }

        sg_tMenuManage.pMenuCtrl->showBaseItem = tMenuShow.showBaseItem;
    }

//    return 0;
}
