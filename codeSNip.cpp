        fgBrush.setColor(QColor(0,0,0,20));
        //setBackgroundBrush(QImage(streamThd->frameptr,streamThd->framewidth, streamThd->frameheight, QImage::Format_RGB888));
        //bgBrush.setColor(Qt::white);
        setBackgroundBrush(bgBrush);
        painter->setBrush(fgBrush);
        for(int i=0;i<streamThd->gt_N;i++)
        {
            REAL* bb=streamThd->targetBB.cur_frame_ptr;
            REAL left = bb[i*4+0], top = bb[i*4+1], right = bb[i*4+2], bottom = bb[i*4+3];
            if(streamThd->targetBB.len>0)
            {
                REAL* bbt_1=streamThd->targetBB.cur_frame_ptr-streamThd->targetBB.frame_step_size;
                REAL left1 = bbt_1[i*4+0], top1 = bbt_1[i*4+1], right1 = bbt_1[i*4+2], bottom1 = bbt_1[i*4+3];
                REAL displc=abs(left-left1)+abs(top-top1);
                //if(displc>0)
                if(i==8)
                {
                    painter->setBrush(fgBrush);
                }
                else
                {
                    painter->setBrush(QBrush());
                }
            }
            //painter->setPen(Qt::NoPen);
            painter->setPen(QColor(feat_colos[i%6][0],feat_colos[i%6][1],feat_colos[i%6][2]));
            painter->drawRect(left,top,right-left,bottom-top);
            //painter->setBrush();
        }
        for(int i=0;i<streamThd->gt_N;i++)
        {
            REAL* bb=streamThd->targetBB.cur_frame_ptr;
            REAL left = bb[i*4+0], top = bb[i*4+1], right = bb[i*4+2], bottom = bb[i*4+3];
            QPen txtpen(QColor(feat_colos[i%6][0],feat_colos[i%6][1],feat_colos[i%6][2]));
            painter->setPen(txtpen);
            sprintf(viewstrbuff,"%d\0",i);
            painter->setFont(QFont("Arial", 8));
            painter->setBrush(QBrush(QColor(0,0,0)));
            painter->drawText(QRectF (left,top,12,12), Qt::AlignRight|Qt::AlignTop,viewstrbuff );
        }