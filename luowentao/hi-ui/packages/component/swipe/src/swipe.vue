<template>
  <div>
    <div class="swipe">
      <slot name="swipe-item"></slot>
      <div class="indicators">
        <li v-for="(item,index) in size" :key="index" :class="nowItemId==index?'now':''"></li>
      </div>
    </div>

  </div>
</template>

<script>
export default {
  name: "HiSwipe",
  props: {
    height: {
      type: Number,
      default: 200
    }
  },
  data() {
    return {
      nowItemId: 0,
      startX: 0,
      endX: 0,
      moveX: 0,
      offsetX: 0,
      offset: 0,
      size: 0,
      width:0
    }
  },
  mounted() {
    this.init();
    window.addEventListener('resize', () => {
      console.log(1111)
      let swipe = document.getElementsByClassName('swipe').item(0);
      let swipeItem = swipe.getElementsByTagName('div').item(0).getElementsByClassName('swipe-item')

      for (let i = 0; i < swipeItem.length; i++) {
        swipeItem[i].style.height = this.height + 'px';
        swipeItem[i].style.width = swipe.clientWidth + 'px';
        // swipeItem[i].style.lineHeight = this.height + 'px';
        swipeItem[i].style.flexShrink = '0';
      }
      this.size = swipeItem.length
      this.width = swipeItem[0].clientWidth;
      this.move(swipe, this.width, true)
    })
  },
  methods: {
    init() {
      let swipe = document.getElementsByClassName('swipe').item(0);
      let swipeItem = swipe.getElementsByTagName('div').item(0).getElementsByClassName('swipe-item')

      for (let i = 0; i < swipeItem.length; i++) {
        swipeItem[i].style.height = this.height + 'px';
        swipeItem[i].style.width = swipe.clientWidth + 'px';
        // swipeItem[i].style.lineHeight = this.height + 'px';
        swipeItem[i].style.flexShrink = '0';
      }
      this.size = swipeItem.length
      this.width = swipeItem[0].clientWidth;
      swipe.getElementsByTagName('div').item(0).style.width = (swipeItem.length * this.width) + 'px';
      swipe.getElementsByTagName('div').item(0).style.display = 'flex';
      swipe.getElementsByTagName('div').item(0).addEventListener('touchstart', (e) => {
        this.reset();
        this.startX = e.touches[0].pageX;
      }, false)
      swipe.getElementsByTagName('div').item(0).addEventListener('touchmove', (e) => {
        this.endX = e.touches[0].clientX;
        this.moveX = this.endX - this.startX;
        this.offsetX = Math.abs(this.moveX);
        swipe.getElementsByTagName('div').item(0).style.transitionDuration = '0ms';
        this.move(swipe, this.width, false);
      }, false)
      swipe.getElementsByTagName('div').item(0).addEventListener('touchend', (e) => {
        const shouldSwipe = this.offsetX > this.width / 4;
        swipe.getElementsByTagName('div').item(0).style.transitionDuration = '500ms';
        if (shouldSwipe) {
          if (this.moveX > 0 && this.nowItemId > 0) {
            this.nowItemId--;
          } else if (this.moveX < 0 && this.nowItemId < (swipeItem.length - 1)) {
            this.nowItemId++;
          }
          this.move(swipe, this.width, shouldSwipe);
        } else {
          this.offsetX = 0;
          this.move(swipe, this.width, shouldSwipe);
        }
      }, false)
    },
    reset() {
      this.startX = 0;
      this.endX = 0;
      this.moveX = 0;
      this.offsetX = 0;
    },
    move(swipe, width, isSwitch) {
      if (isSwitch) {
        swipe.getElementsByTagName('div').item(0).style.transform = `translateX(${-((this.nowItemId * width))}px)`;
      } else {
        if (this.moveX > 0) {
          swipe.getElementsByTagName('div').item(0).style.transform = `translateX(${(this.offsetX + (-this.nowItemId * width))}px)`;
        } else {
          swipe.getElementsByTagName('div').item(0).style.transform = `translateX(${-(this.nowItemId * width) - this.offsetX}px)`;
        }
      }
    }
  }
}
</script>

<style scoped>
.swipe {
  width: 100%;
  overflow: hidden;
}

.indicators {
  margin-top: 10px;
  font-size: 0;
  text-align: center;
}

.indicators li {
  width: 7px;
  height: 7px;
  display: inline-block;
  background: rgba(0, 0, 0, 0.1);
  border-radius: 50%;
  margin-left: 5px;
}

.now {
  width: 12px !important;
  border-radius: 12px !important;
  background: #0A59F7 !important;
}
</style>
