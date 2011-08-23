/*
 * @(#)Guiffy application - ErrorMsg.java        3-Sep-2004
 *
 * Copyright (c) 1998 - 2004  Guiffy Software.  All Rights Reserved.
 * by: Bill W. Ritcher
 *
 */


package com.guiffy.guiffy;


/**
 * ErrorMsg is the Error Message retriever Guiffy
 *
 * @version 6.4 3-Sep-2004
 * @author Bill W. Ritcher
 */
public class ErrorMsg {

  public static final int CF1 = 1;
  public static final int CF2 = 2;
  public static final int CFP = 3;
  public static final int CR1 = 4;
  public static final int CR2 = 5;
  public static final int CRP = 6;

  public String ema[];

    /**
     * Creates an ErrorMsg retriever
     */
    public ErrorMsg( ) {
        ema = new String[10];
        ema[CF1] = ReBu.grbGet("cd_CantFind1.text");
        ema[CF2] = ReBu.grbGet("cd_CantFind2.text");
        ema[CFP] = ReBu.grbGet("cd_CantFindP.text");
        ema[CR1] = ReBu.grbGet("cd_CantRead1.text");
        ema[CR2] = ReBu.grbGet("cd_CantRead2.text");
        ema[CRP] = ReBu.grbGet("cd_CantReadP.text");
    }

    /**
     * Returns the requested Error Message.
     * @param errnum  the Error Number
     * @return the Error Message String 
     */
    public String getErrorMsg( int ernum) {

        return ema[ernum];
    }

}
