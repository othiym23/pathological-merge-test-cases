
        // IF RVM for Parent Attention or Folded View
        if(conBegi != 0) {
          if(conBegi > metrics_Array[0])return;
          if(conEndi < metrics_Array[0])return;
          if(attEndi == 0 && metrics_Array[0] == attBegi+1) {
                if(folBegi == 0) {
                    printls( "|" + EOL );
                }
          }
          if(attEndi != 0) {
            if(attBegi+1 <= metrics_Array[0] &&
               attEndi+1 >= metrics_Array[0] ) {
                printls( "<" + fir_SLine + EOL );
                return;
            }
          }
        }

